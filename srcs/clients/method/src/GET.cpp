
#include "GET.hpp"

#include <dirent.h>
#include <unistd.h>

#include <fstream>
#include <sstream>

#include "Utils.hpp"

GET::GET() {}
GET::GET(std::string& request) : AMethod(request), _autoIndex(false) {}
GET::~GET() {}

void GET::doRequest(void) {
  std::string pathIndex;

  pathIndex = this->_path;
  std::cout << " -------------- pathIndex: " << pathIndex << std::endl;
  std::cout << " -------------- this : " << this->_path << std::endl;
  // if (pathIndex[pathIndex.size() - 1] == '/') {
  //   pathIndex += "index.html";
  // }
  if (_matchedLocation->getIndex() != "") {
    this->_path += _matchedLocation->getIndex();
  }
  if (access(pathIndex.c_str(), R_OK) == 0) {
    this->_statusCode = OK;
    prepareBody(pathIndex);
    if (this->_body == "") {  // 반환할 content가 없음
      this->_statusCode = NO_CONTENT;
    }
  } else if (this->_path[this->_path.size() - 1] == '/' &&
             _matchedLocation->getAutoindex() == "on" &&
             access(pathIndex.c_str(), R_OK) < 0) {
    this->_statusCode = OK;
    prepareFileList(this->_path);
    _autoIndex = true;
  } else {
    throw(this->_statusCode = NOT_FOUND);
  }
}

void GET::appendBody(void) { this->_response += "\r\n" + this->_body; }

void GET::prepareFileList(const std::string& path) {
  DIR* dir;
  struct dirent* ent;

  this->_body = "<html><body>";
  if ((dir = opendir(path.c_str())) != NULL) {
    while ((ent = readdir(dir)) != NULL) {
      this->_body += "<p>";
      this->_body += ent->d_name;
      this->_body += "</p>";
    }
    closedir(dir);
  } else {
    this->_body = "Could not open directory";
    throw(this->_statusCode = FORBIDDEN);
  }
  this->_body += "</body></html>";
}

void GET::prepareBody(const std::string& pathIndex) {
  std::ifstream file(pathIndex.c_str(), std::ios::in);
  std::string buff;
  while (std::getline(file, buff)) this->_body += buff + "\r\n";
  file.close();
}

void GET::createSuccessResponse(void) {
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

/* GET 요청
GET /hello.txt HTTP/1.1
User-Agent: curl/7.16.3 libcurl/7.16.3 OpenSSL/0.9.71 zlib/1.2.3 Host:
www.example.com Accept-Lanuage: en, mi
*/

/* GET 응답
"http://example.com/index.html"
HTTP/1.1 200 OK
Date: Mon, 27 Jul 2009 12:28:53 GMT
Server: Apache
Last-Modified: Wed, 22 Jul 2009 19:15:56 GMT ETag: "34aa387-d-1568eb00"
Accept-Ranges: bytes
Content-Length: 51
Vary: Accept-Encoding
Content-Type: text/plain

Hello World! My payload includes a trailing CRLF.
*/

/*
# : 8081 / test
#> / YoupiBanane / test / youpi.bad_extension
# : 8081 / test /
#> / YoupiBanane / test / youpi.bad_extension
#if autoindex on
#/ YoupiBanane / test / youpi.bad_extension> 실행이 될경우 걍 실행함.
#/ YoupiBanane / test / youpi.bad_extension> \
    실행이 안될 경우.directory를 보여줌.
#/ YoupiBanane / test /<디렉토리를 보여줌, .하위 디렉토리도.
#if autoindex off
#/ YoupiBanane / test / youpi.bad_extension> 실행이 될경우 걍 실행함.
#/ YoupiBanane / test / youpi.bad_extension> 실행이 안될 경우.404 not found
#/ YoupiBanane / test / youpi.bad_extension> 실행이 안될 경우.403 Forbidden< \
    얘가 존재한다는 걸 알아서 soft 404로 처리
*/

/*
HTTP GET 요청에 대한 응답은 주로 클라이언트가 요청한 리소스의 상태와 서버의 처리
결과에 따라 결정됩니다. 다음은 주요 HTTP 상태 코드와 각각의 의미입니다:

- **200 OK:** 클라이언트가 요청한 리소스가 성공적으로 반환되었음을 나타냅니다.
이는 가장 일반적인 GET 요청의 응답입니다. 응답 본문에는 요청한 리소스의 내용이
포함됩니다.

- **204 No Content:** 서버가 요청을 성공적으로 처리했지만 반환할 콘텐츠가 없음을
나타냅니다.

- **301 Moved Permanently:** 요청한 리소스가 영구적으로 새 위치로 이동했음을
나타냅니다. 이 응답에는 `Location` 헤더가 포함되어 새로운 리소스 위치를
나타냅니다.

- **302 Found:** 요청한 리소스가 일시적으로 다른 위치로 이동했음을 나타냅니다.
이 응답에는 `Location` 헤더가 포함되어 새로운 리소스 위치를 나타냅니다.

- **304 Not Modified:** 클라이언트가 조건부 GET 요청을 보냈고, 요청한 리소스가
변경되지 않았음을 나타냅니다. 이 응답은 서버 대역폭을 절약하는 데 도움이 됩니다.

- **400 Bad Request:** 클라이언트의 요청이 서버에서 해석할 수 없거나, 문법적으로
잘못된 경우에 반환됩니다.

- **401 Unauthorized:** 클라이언트가 인증되지 않았거나, 인증에 실패한 경우에
반환됩니다.

- **403 Forbidden:** 클라이언트가 요청한 리소스에 대한 접근 권한이 없는 경우에
반환됩니다.

- **404 Not Found:** 클라이언트가 요청한 리소스가 서버에서 찾을 수 없는 경우에
반환됩니다.

- **500 Internal Server Error:** 서버 내부에서 예상치 못한 오류가 발생한 경우에
반환됩니다.

서버는 이러한 상태 코드와 함께 응답 본문에 상세한 오류 메시지나 리소스에 대한
정보를 제공할 수 있습니다. 이는 클라이언트가 서버의 응답을 이해하고, 적절한
조치를 취할 수 있도록 돕습니다.
*/