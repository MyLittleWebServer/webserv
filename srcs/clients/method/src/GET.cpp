
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
  std::string pathIndex = makePathIndex(dts);

#ifdef DEBUG_MSG
  std::cout << " -- this : " << *dts.path << std::endl;
  std::cout << " -- this : " << pathIndex << std::endl;
#endif

  if (checkFilePath(*dts.path)) {
    prepareBody(*dts.path, response);
  } else if (checkFilePath(pathIndex)) {
    prepareBody(pathIndex, response);
  } else if (checkDirectoryPath(pathIndex, dts)) {
    prepareFileList(*dts.path, dts, response);
  } else {
    throw(*dts.statusCode = NOT_FOUND);
  }
  *dts.statusCode = OK;
  if (response.getBody().empty()) {
    *dts.statusCode = NO_CONTENT;
  }
}

std::string GET::makePathIndex(RequestDts& dts) {
  std::string pathIndex = *dts.path;
  if (dts.matchedLocation->getIndex() != "") {
    pathIndex += (*dts.path)[dts.path->size() - 1] == '/'
                     ? dts.matchedLocation->getIndex()
                     : "/" + dts.matchedLocation->getIndex();
  }
  return (pathIndex);
}

bool GET::checkFilePath(const std::string& path) {
  return (access(path.c_str(), R_OK) == 0 && path[path.size() - 1] != '/');
}

bool GET::checkDirectoryPath(const std::string& pathIndex, RequestDts& dts) {
  return (access(pathIndex.c_str(), R_OK) < 0 &&
          access((*dts.path).c_str(), R_OK) == 0 &&
          (*dts.path)[dts.path->size() - 1] == '/' &&
          dts.matchedLocation->getAutoindex() == "on");
}

std::vector<std::string> GET::getFileList(const std::string& path,
                                          RequestDts& dts,
                                          std::vector<std::string>& files) {
  DIR* dir;
  struct dirent* ent;

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
    std::vector<std::string> files;
    getFileList(path, dts, files);
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

void GET::getContentType(const std::string& path, IResponse& response) {
  std::string extension = path.substr(path.find_last_of(".") + 1);
  MimeTypesConfig& config = dynamic_cast<MimeTypesConfig&>(
      Config::getInstance().getMimeTypesConfig());
  try {
    std::cout << "extension::" << extension << std::endl;
    response.setHeaderField("Content-Type", config.getVariable(extension));
    return;
  } catch (std::exception& e) {
    std::cout << "find :: ";
    std::map<std::string, std::string>::iterator it;
    for (it = config._data.begin(); it != config._data.end(); ++it) {
      if (it->first.find(extension) != std::string::npos) {
        std::cout << it->second << std::endl;
        response.setHeaderField("Content-Type", it->second);
      }
    }
    response.setHeaderField("Content-Type", "text/plain");
  }
}
