
#include "GET.hpp"

#include <dirent.h>
#include <unistd.h>

#include <fstream>
#include <sstream>
#include <vector>

#include "Utils.hpp"

GET::GET() {}
GET::GET(std::string& request) : AMethod(request), _contentType("text/plain") {}
GET::~GET() {}

void GET::doRequest(void) {
  std::string pathIndex;

  pathIndex = this->_path;
  if (this->_matchedLocation->getIndex() != "") {
    pathIndex += this->_path[this->_path.size() - 1] == '/'
                     ? this->_matchedLocation->getIndex()
                     : "/" + this->_matchedLocation->getIndex();
  }
#ifdef DEBUG_MSG
  std::cout << " -- this : " << this->_path << std::endl;
  std::cout << " -- this : " << pathIndex << std::endl;
#endif
  if (access(this->_path.c_str(), R_OK) == 0 &&
      this->_path[this->_path.size() - 1] != '/') {
    this->_statusCode = OK;
    prepareBody(this->_path);
  } else if (access(pathIndex.c_str(), R_OK) == 0 &&
             pathIndex[pathIndex.size() - 1] != '/') {
    this->_statusCode = OK;
    prepareBody(pathIndex);
  } else if (access(pathIndex.c_str(), R_OK) < 0 &&
             this->_path[this->_path.size() - 1] == '/' &&
             _matchedLocation->getAutoindex() == "on") {
    this->_statusCode = OK;
    prepareFileList(this->_path);
  } else {
    throw(this->_statusCode = NOT_FOUND);
  }
  if (this->_body == "") {
    this->_statusCode = NO_CONTENT;
  }
}

void GET::appendBody(void) { this->_response += "\r\n" + this->_body; }

std::vector<std::string> GET::getFileList(const std::string& path) {
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
    throw(this->_statusCode = FORBIDDEN);
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

void GET::prepareFileList(const std::string& path) {
  try {
    std::vector<std::string> files = getFileList(path);
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

void GET::createSuccessResponse(void) {
  assembleResponseLine();
  
  this->_response += getCurrentTime();
  this->_response += "\r\n";
  this->_response += "Content-Type: ";
  this->_response += validateContentType();
  this->_response += "\r\n";
  this->_response += "Content-Length: ";
  this->_response += itos(this->_body.size());
  this->_response += "\r\n";
  this->appendBody();
  #ifdef DEBUG_MSG_BODY
  std::cout << this->_response << "\n";
  #endif
  this->_responseFlag = true;
}

std::string GET::validateContentType() {
  if (_contentType == "text/html" || _contentType == "text/css" || _contentType == "application/json") {
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
