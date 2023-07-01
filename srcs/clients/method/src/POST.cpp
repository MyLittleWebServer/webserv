#include "POST.hpp"

#include <unistd.h>

#include <fstream>

#include "Utils.hpp"

POST::POST(void) {}

POST::POST(std::string& request)
    : AMethod(request), _contentType("text/plain") {}

POST::~POST(void) {}

void POST::doRequest() {
  std::string pathIndex;

  pathIndex = this->_path;
  if (this->_matchedLocation->getIndex() != "") {
    pathIndex += this->_path[this->_path.size() - 1] == '/'
                     ? this->_matchedLocation->getIndex()
                     : "/" + this->_matchedLocation->getIndex();
  }
  std::istringstream ss(this->_headerFields["content-length"]);
  ss >> this->_body;
  if (this->_body.size() == 0)
    throw(this->_statusCode = BAD_REQUEST);
  else if (this->_body.size() > _matchedLocation->getLimitClientBodySize())
    throw(this->_statusCode = REQUEST_ENTITY_TOO_LARGE);
  this->generateResource();
}

void POST::generateResource() {
  if (access(this->_path.c_str(), F_OK) < 0)
    throw(this->_statusCode = FORBIDDEN);
  std::ofstream file(this->_path.c_str(), std::ios::out);
  if (!file.is_open()) throw(this->_statusCode = INTERNAL_SERVER_ERROR);
  std::list<std::string>::const_iterator it = this->_linesBuffer.begin();
  std::list<std::string>::const_iterator end = this->_linesBuffer.end();
  while (it != end) {
    file << *it++;
    if (file.fail()) throw(this->_statusCode = INTERNAL_SERVER_ERROR);
  }
  file.close();

  this->_statusCode = CREATED;
}

void POST::appendBody(void) { this->_response += "\r\n" + this->_body; }

void POST::createSuccessResponse(void) {
  assembleResponseLine();
  this->_response += getCurrentTime();
  this->_response += "\r\n";
  this->_response += "Content-Type: text/html; charset=UTF-8\r\n";
  this->_response += "Content-Length: ";
  this->_response += itos(this->_body.size());
  this->_response += "\r\n";
  this->appendBody();
  std::cout << this->_response << "\n";
  this->_responseFlag = true;
}

std::string POST::validateContentType(void) {
  if (_contentType == "text/plain" || _contentType == "multipart/form-data" ||
      _contentType == "application/x-www-form-urlencoded") {
    return _contentType + "; charset=UTF-8";
  }
  return _contentType;
}

// GET::GET() {}
// GET::GET(std::string& request) : AMethod(request), _contentType("text/plain")
// {} GET::~GET() {}

// void GET::doRequest(void) {
//   std::string pathIndex;

//   pathIndex = this->_path;
//   if (this->_matchedLocation->getIndex() != "") {
//     pathIndex += this->_path[this->_path.size() - 1] == '/'
//                      ? this->_matchedLocation->getIndex()
//                      : "/" + this->_matchedLocation->getIndex();
//   }
// #ifdef DEBUG_MSG
//   std::cout << " -- this : " << this->_path << std::endl;
//   std::cout << " -- this : " << pathIndex << std::endl;
// #endif
//   if (access(this->_path.c_str(), R_OK) == 0 &&
//       this->_path[this->_path.size() - 1] != '/') {
//     this->_statusCode = OK;
//     prepareBody(this->_path);
//   } else if (access(pathIndex.c_str(), R_OK) == 0 &&
//              pathIndex[pathIndex.size() - 1] != '/') {
//     this->_statusCode = OK;
//     prepareBody(pathIndex);
//   } else if (access(pathIndex.c_str(), R_OK) < 0 &&
//              this->_path[this->_path.size() - 1] == '/' &&
//              _matchedLocation->getAutoindex() == "on") {
//     this->_statusCode = OK;
//     prepareFileList(this->_path);
//   } else {
//     throw(this->_statusCode = NOT_FOUND);
//   }
//   if (this->_body == "") {
//     this->_statusCode = NO_CONTENT;
//   }
// }

// void GET::appendBody(void) { this->_response += "\r\n" + this->_body; }

// std::vector<std::string> GET::getFileList(const std::string& path) {
//   DIR* dir;
//   struct dirent* ent;
//   std::vector<std::string> files;

//   if ((dir = opendir(path.c_str())) != NULL) {
//     while ((ent = readdir(dir)) != NULL) {
//       if (ent->d_name[0] == '.') continue;
//       if (ent->d_type == DT_DIR)
//         files.push_back(ent->d_name + std::string("/"));
//       else
//         files.push_back(ent->d_name);
//     }
//     closedir(dir);
//   } else {
//     throw(this->_statusCode = FORBIDDEN);
//   }
//   return files;
// }

// std::string GET::generateHTML(const std::vector<std::string>& files) {
//   std::string html = "<html><body>";
//   for (std::vector<std::string>::const_iterator it = files.begin();
//        it != files.end(); ++it) {
//     html += "<a href=\"" + *it + "\">" + *it + "</a><br>";
//   }
//   html += "</body></html>";
//   return html;
// }

// void GET::prepareFileList(const std::string& path) {
//   try {
//     std::vector<std::string> files = getFileList(path);
//     this->_body = generateHTML(files);
//     this->_contentType = "text/html";
//   } catch (int statusCode) {
//     this->_body = "Could not open directory";
//     throw(statusCode);
//   }
// }

// void GET::prepareBody(const std::string& path) {
//   getContentType(path);
//   if (this->_contentType == "text/html" || this->_contentType == "text/css"
//   ||
//       this->_contentType == "application/json") {
//     prepareTextBody(path);
//   } else {
//     prepareBinaryBody(path);
//   }
// }

// void GET::prepareTextBody(const std::string& path) {
//   std::ifstream file(path.c_str(), std::ios::in);
//   std::string buff;
//   while (std::getline(file, buff)) this->_body += buff + "\r\n";
//   file.close();
// }

// void GET::prepareBinaryBody(const std::string& path) {
//   std::ifstream file(path.c_str(), std::ios::binary);
//   std::stringstream buffer;
//   buffer << file.rdbuf();
//   this->_body = buffer.str();
//   file.close();
// }

// void GET::createSuccessResponse(void) {
//   assembleResponseLine();

//   this->_response += getCurrentTime();
//   this->_response += "\r\n";
//   this->_response += "Content-Type: ";
//   this->_response += validateContentType();
//   this->_response += "\r\n";
//   this->_response += "Content-Length: ";
//   this->_response += itos(this->_body.size());
//   this->_response += "\r\n";
//   this->appendBody();
//   #ifdef DEBUG_MSG_BODY
//   std::cout << this->_response << "\n";
//   #endif
//   this->_responseFlag = true;
// }

// std::string GET::validateContentType() {
//   if (_contentType == "text/html" || _contentType == "text/css" ||
//   _contentType == "application/json") {
//     return _contentType + "; charset=UTF-8";
//   }
//   return _contentType;
// }

// void GET::getContentType(const std::string& path) {
//   std::string extension = path.substr(path.find_last_of(".") + 1);
//   MimeTypesConfig& config = dynamic_cast<MimeTypesConfig&>(
//       Config::getInstance().getMimeTypesConfig());
//   try {
//     std::cout << "extension::" << extension << std::endl;
//     _contentType = config.getVariable(extension);
//     return;
//   } catch (std::exception& e) {
//     std::cout << "find :: ";
//     std::map<std::string, std::string>::iterator it;
//     for (it = config._data.begin(); it != config._data.end(); ++it) {
//       if (it->first.find(extension) != std::string::npos) {
//         std::cout << it->second << std::endl;
//         _contentType = it->second;
//       }
//     }
//     _contentType = "text/plain";
//   }
// }

/* POST 요청
POST /api/users HTTP/1.1
Host: example.com
Content-Type: application/json
Content-Length: 43

{"username": "john_doe", "password": "secret123"}*/

/* POST 응답
HTTP/1.1 201 Created
Content-Type: application/json
Content-Length: 27

{"message": "User created"}

/*

HTTP POST 요청의 본문(body)
은 서버에 데이터를 전송하는 데
        사용됩니다.이 데이터는 서버에서 여러 가지 방법으로 처리될 수 있습니다.

- 클라이언트에서 전송한 데이터가 데이터베이스에 저장되는 경우
- 클라이언트에서 업로드한 파일이 서버의 파일 시스템에 저장되는 경우
- 클라이언트에서 전송한 명령이 서버에서 실행되는 경우

        따라서 POST 요청의 본문이 그대로 파일에 저장되는지는 서버의 로직에
따라 다릅니다.예를 들어, 클라이언트가 파일을 업로드하는 경우 서버는 그 파일을
디스크에 저장할 수 있습니다.그러나 POST 요청의 본문이 JSON과 같은 데이터를
포함하고 있다면, 이 데이터는 일반적으로 데이터베이스에 저장되거나 서버의
로직에 따라 처리됩니다.

    그러므로 일반적으로 HTTP POST 요청의 본문이 그대로 파일에 저장되지는
않지만, 서버의 구현에 따라 그렇게 처리될 수도 있습니다 .이는 서버가
클라이언트에서 보내온 데이터를 어떻게 처리하느냐에 따라 달라집니다.
*/

/*
POST 요청에 대한 응답으로 서버가 반환할 수 있는 상태 코드는 다음과 같습니다:

1. `200 OK`: 요청이 성공적으로 처리되었으며, 서버가 응답 본문에 추가 정보를
제공하고 있다는 것을 나타냅니다.

2. `201 Created`: 요청이 성공적으로 처리되었고, 새 리소스가 생성되었다는 것을
나타냅니다. 주로 POST 요청으로 새 리소스를 생성했을 때 사용됩니다.

3. `202 Accepted`: 요청은 수락되었지만, 아직 처리되지 않았다는 것을
나타냅니다. 나중에 처리될 것이며, 응답은 처리가 완료되면 가능한 빠르게 반환될
것입니다.

4. `204 No Content`: 요청이 성공적으로 처리되었지만, 서버가 반환할 추가 내용이
없다는 것을 나타냅니다. 일반적으로 POST 요청에 대한 응답으로 사용되며, 이 경우
클라이언트는 현재 보고 있는 페이지를 새로고침하지 않아도 됩니다.

5. `400 Bad Request`: 클라이언트의 요청이 유효하지 않아 처리할 수 없다는 것을
나타냅니다. 이는 클라이언트 측에서 잘못된 데이터를 제출했을 때 발생합니다.

6. `401 Unauthorized`: 클라이언트가 인증되지 않아 요청을 수행할 권한이 없다는
것을 나타냅니다.

7. `403 Forbidden`: 클라이언트가 요청한 리소스에 대한 접근 권한이 없다는 것을
나타냅니다.

8. `404 Not Found`: 요청한 리소스를 찾을 수 없다는 것을 나타냅니다.

9. `413 Request Entity Too Large`: 요청 본문의 크기가 서버가 처리할 수 있는
크기를 초과했다는 것을 나타냅니다.

10. `500 Internal Server Error`: 서버 내부에서 처리 중에 오류가 발생했다는
것을 나타냅니다.

11. `501 Not Implemented`: 서버가 요청 메소드를 지원하지 않아 처리할 수 없다는
것을 나타냅니다.

이상과 같은 상태 코드 외에도 다양한 HTTP 상태 코드가 있으며, 각 코드는
클라이언트에게 서버가 요청을 어떻게 처리했는지에 대한 정보를 제공합니다.
*/