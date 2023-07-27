
#include "GET.hpp"

#include <dirent.h>
#include <unistd.h>

#include <cstdlib>
#include <fstream>
#include <sstream>
#include <vector>

#include "Kqueue.hpp"
#include "Utils.hpp"

GET::GET() : IMethod() {}
GET::~GET() {}

void GET::doRequest(RequestDts& dts, IResponse& response) {
  response.setHeaderField("Content-Type", "text/plain");
  std::string pathIndex;

  pathIndex = *dts.path;
  if ((*dts.matchedLocation)->getIndex() != "") {
    pathIndex += (*dts.path)[dts.path->size() - 1] == '/'
                     ? (*dts.matchedLocation)->getIndex()
                     : "/" + (*dts.matchedLocation)->getIndex();
  }
#ifdef DEBUG_MSG
  std::cout << " -- this : " << *dts.path << std::endl;
  std::cout << " -- this : " << pathIndex << std::endl;
#endif

  if (access(dts.path->c_str(), R_OK) == 0 &&
      (*dts.path)[dts.path->size() - 1] != '/') {
    *dts.statusCode = E_200_OK;
    prepareBody(*dts.path, response);
  } else if (access(pathIndex.c_str(), R_OK) == 0 &&
             pathIndex[pathIndex.size() - 1] != '/') {
    *dts.statusCode = E_200_OK;
    prepareBody(pathIndex, response);
  } else if (access(pathIndex.c_str(), R_OK) < 0 &&
             (*dts.path)[dts.path->size() - 1] == '/' &&
             (*dts.matchedLocation)->getAutoindex() == "on") {
    *dts.statusCode = E_200_OK;
    prepareFileList(*dts.path, dts, response);
  } else {
    throw(*dts.statusCode = E_404_NOT_FOUND);
  }
  if (response.getBody().empty()) {
    *dts.statusCode = E_204_NO_CONTENT;
  }
}

std::vector<std::string> GET::getFileList(const std::string& path,
                                          RequestDts& dts) {
  DIR* dir;
  struct dirent* ent;
  std::vector<std::string> files;

  if ((*dts.originalPath)[(*dts.originalPath).size() - 1] != '/')
    *dts.originalPath += '/';
  if ((dir = opendir(path.c_str())) != NULL) {
    while ((ent = readdir(dir)) != NULL) {
      if (ent->d_name[0] == '.') continue;
      if (ent->d_type == DT_DIR)
        files.push_back((*dts.originalPath + ent->d_name + '/'));
      else
        files.push_back((*dts.originalPath + ent->d_name));
      std::cout << "name: " << ent->d_name << '\n';
    }
    closedir(dir);
  } else {
    throw(*dts.statusCode = E_404_NOT_FOUND);
  }
  return files;
}

std::string GET::generateHTML(const std::vector<std::string>& files) {
  std::string html = "<html><body>";
  for (std::vector<std::string>::const_iterator it = files.begin();
       it != files.end(); ++it) {
    html += "<a href=\"" + *it + "\">" + *it + "</a><br>";
  }
  html += "</body></html>";
  return html;
}

void GET::fileHandler(const std::string& path) {
  size_t fd = size_t(std::fopen(path.c_str(), "r"));
  (void)fd;
  // error handling code
}

void GET::prepareFileList(const std::string& path, RequestDts& dts,
                          IResponse& response) {
  try {
    std::vector<std::string> files = getFileList(path, dts);
    response.setBody(generateHTML(files));
    response.setHeaderField("Content-Type", "text/html");
  } catch (int statusCode) {
    response.setBody("Could not open directory");
    throw(statusCode);
  }
}

void GET::prepareBody(const std::string& path, IResponse& response) {
  getContentType(path, response);
  const std::string& value = response.getFieldValue("Content-Type");
  if (value == "text/html" || value == "text/css" ||
      value == "application/json") {
    prepareTextBody(path, response);
  } else {
    prepareBinaryBody(path, response);
  }
}

void GET::prepareTextBody(const std::string& path, IResponse& response) {
  std::ifstream file(path.c_str(), std::ios::in);
  std::string buff;
  while (std::getline(file, buff)) {
    response.addBody(buff);
    response.addBody("\r\n");
  }
  file.close();
}

void GET::prepareBinaryBody(const std::string& path, IResponse& response) {
  std::ifstream file(path.c_str(), std::ios::binary);
  std::stringstream buffer;
  buffer << file.rdbuf();
  response.addBody(buffer.str());
  file.close();
}

void GET::createSuccessResponse(IResponse& response) {
  validateContentType(response);
  response.setHeaderField("Content-Length", itos(response.getBody().size()));
  response.assembleResponse();
#ifdef DEBUG_MSG_BODY
  std::cout << response.getResponse() << "\n";
#endif
  response.setResponseParsed();
}

void GET::validateContentType(IResponse& response) {
  const std::string& value = response.getFieldValue("Content-Type");
  if (value == "text/html" || value == "text/css" || value == "text/xml" ||
      value == "text/mathml" || value == "text/plain" ||
      value == "text/vnd.sun.j2me.app-descriptor" ||
      value == "text/vnd.wap.wml" || value == "text/x-component" ||
      value == "application/javascript" || value == "application/atom+xml" ||
      value == "application/rss+xml" || value == "application/json" ||
      value == "application/x-perl" || value == "application/json" ||
      value == "application/xhtml+xml" || value == "application/x-tcl" ||
      value == "application/xspf+xml") {
    response.setHeaderField("Content-Type", value + "; charset=UTF-8");
  }
}

void GET::getContentType(const std::string& path, IResponse& response) {
  std::string extension = path.substr(path.find_last_of(".") + 1);
  MimeTypesConfig& config = dynamic_cast<MimeTypesConfig&>(
      Config::getInstance().getMimeTypesConfig());
  try {
    std::cout << "extension::" << extension << std::endl;
    response.setHeaderField("Content-Type", config.getVariable(extension));
    return;
  } catch (ExceptionThrower::InvalidConfigException& e) {
    response.setHeaderField("Content-Type", "application/octet-stream");
  }
}
