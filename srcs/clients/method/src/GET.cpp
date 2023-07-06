
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
  if (dts.matchedLocation->getIndex() != "") {
    pathIndex += (*dts.path)[dts.path->size() - 1] == '/'
                     ? dts.matchedLocation->getIndex()
                     : "/" + dts.matchedLocation->getIndex();
  }
#ifdef DEBUG_MSG
  std::cout << " -- this : " << this->_path << std::endl;
  std::cout << " -- this : " << pathIndex << std::endl;
#endif
  if (access(dts.path->c_str(), R_OK) == 0 &&
      (*dts.path)[dts.path->size() - 1] != '/') {
    *dts.statusCode = OK;
    // return fileHandler(*dts.path);
    prepareBody(*dts.path, response);
  } else if (access(pathIndex.c_str(), R_OK) == 0 &&
             pathIndex[pathIndex.size() - 1] != '/') {
    *dts.statusCode = OK;
    // return fileHandler(pathIndex);
    prepareBody(*dts.path, response);
  } else if (access(pathIndex.c_str(), R_OK) < 0 &&
             (*dts.path)[dts.path->size() - 1] == '/' &&
             dts.matchedLocation->getAutoindex() == "on") {
    *dts.statusCode = OK;
    prepareFileList(*dts.path, dts, response);
    // return 0;
  } else {
    throw(*dts.statusCode = NOT_FOUND);
  }
  if (response.getBody().empty()) {
    *dts.statusCode = NO_CONTENT;
  }
}

std::vector<std::string> GET::getFileList(const std::string& path,
                                          RequestDts& dts) {
  DIR* dir;
  struct dirent* ent;
  std::vector<std::string> files;

  if ((dir = opendir(path.c_str())) != NULL) {
    while ((ent = readdir(dir)) != NULL) {
      if (ent->d_name[0] == '.') continue;
      if (ent->d_type == DT_DIR)
        files.push_back(ent->d_name + std::string("/"));
      else
        files.push_back(ent->d_name);
    }
    closedir(dir);
  } else {
    throw(*dts.statusCode = FORBIDDEN);
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
  getContentType(path);
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
  response.setHeaderField("Date", getCurrentTime());
  response.setHeaderField("Content-Length", itos(response.getBody().size()));
  response.assembleResponse();
#ifdef DEBUG_MSG_BODY
  std::cout << response.getResponse() << "\n";
#endif
  response.setResponseParsed();
}

void GET::validateContentType(IResponse& response) {
  const std::string& value = response.getFieldValue("Content-Type");
  if (value == "text/html" || value == "text/css" ||
      value == "application/json") {
    response.setHeaderField("Content-Type", value + "; charset=UTF-8");
  }
}

void GET::getContentType(const std::string& path) {
  std::string extension = path.substr(path.find_last_of(".") + 1);
  MimeTypesConfig& config = dynamic_cast<MimeTypesConfig&>(
      Config::getInstance().getMimeTypesConfig());
  try {
    std::cout << "extension::" << extension << std::endl;
    _contentType = config.getVariable(extension);
    return;
  } catch (std::exception& e) {
    std::cout << "find :: ";
    std::map<std::string, std::string>::iterator it;
    for (it = config._data.begin(); it != config._data.end(); ++it) {
      if (it->first.find(extension) != std::string::npos) {
        std::cout << it->second << std::endl;
        _contentType = it->second;
      }
    }
    _contentType = "text/plain";
  }
}
