
#include "GET.hpp"

#include <dirent.h>
#include <unistd.h>

#include <cstdlib>
#include <fstream>
#include <sstream>
#include <vector>

#include "Kqueue.hpp"
#include "Utils.hpp"

GET::GET() : IMethod(), _contentType("text/plain") {}
GET::~GET() {}

void GET::doRequest(RequestDts& dts) {
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
    return fileHandler(*dts.path);
  } else if (access(pathIndex.c_str(), R_OK) == 0 &&
             pathIndex[pathIndex.size() - 1] != '/') {
    *dts.statusCode = OK;
    return fileHandler(pathIndex);
  } else if (access(pathIndex.c_str(), R_OK) < 0 &&
             (*dts.path)[dts.path->size() - 1] == '/' &&
             dts.matchedLocation->getAutoindex() == "on") {
    *dts.statusCode = OK;
    prepareFileList(*dts.path, dts);
    // return 0;
  } else {
    throw(*dts.statusCode = NOT_FOUND);
  }
  if (this->_body == "") {
    *dts.statusCode = NO_CONTENT;
  }
}

void GET::appendBody(IResponse& response) {
  response.addResponse("\r\n" + this->_body);
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

void GET::prepareFileList(const std::string& path, RequestDts& dts) {
  try {
    std::vector<std::string> files = getFileList(path, dts);
    this->_body = generateHTML(files);
    this->_contentType = "text/html";
  } catch (int statusCode) {
    this->_body = "Could not open directory";
    throw(statusCode);
  }
}

void GET::prepareBody(const std::string& path) {
  getContentType(path);
  if (this->_contentType == "text/html" || this->_contentType == "text/css" ||
      this->_contentType == "application/json") {
    prepareTextBody(path);
  } else {
    prepareBinaryBody(path);
  }
}

void GET::prepareTextBody(const std::string& path) {
  std::ifstream file(path.c_str(), std::ios::in);
  std::string buff;
  while (std::getline(file, buff)) this->_body += buff + "\r\n";
  file.close();
}

void GET::prepareBinaryBody(const std::string& path) {
  std::ifstream file(path.c_str(), std::ios::binary);
  std::stringstream buffer;
  buffer << file.rdbuf();
  this->_body = buffer.str();
  file.close();
}

void GET::createSuccessResponse(IResponse& response) {
  response.assembleResponseLine();
  response.addResponse(getCurrentTime());
  response.addResponse("\r\n");
  response.addResponse("Content-Type: ");
  response.addResponse(validateContentType());
  response.addResponse("\r\n");
  response.addResponse("Content-Length: ");
  response.addResponse(itos(this->_body.size()));
  response.addResponse("\r\n");
  this->appendBody(response);
#ifdef DEBUG_MSG_BODY
  std::cout << this->_response << "\n";
#endif
  response.setResponseParsed();
}

std::string GET::validateContentType() {
  if (_contentType == "text/html" || _contentType == "text/css" ||
      _contentType == "application/json") {
    return _contentType + "; charset=UTF-8";
  }
  return _contentType;
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
