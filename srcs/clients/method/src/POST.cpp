#include "POST.hpp"

#include <unistd.h>

#include <fstream>

POST::POST(void) {}

POST::POST(std::string& request) : AMethod(request) {}

POST::~POST(void) {}

void POST::doMethod() {
  int bodySize = 0;
  std::istringstream ss(this->_headerFields["content-length"]);
  ss >> bodySize;

  if (bodySize > _matchedLocation->getLimitClientBodySize()) {
    this->_statusCode = 413;
  } else {
    this->_statusCode = CREATED;
    this->generateFile();
  }
}

void POST::generateFile() {
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
  if (file.fail()) throw(this->_statusCode = INTERNAL_SERVER_ERROR);
}

void POST::createSuccessResponse(void) {
  this->_response += "HTTP/1.1 ";
  this->_response += statusCodes[this->_statusCode].code;
  this->_response += statusCodes[this->_statusCode].message;
  this->_response += "\r\n";
  this->_response += getCurrentTime();

  this->_response += "Content-Type: text/html; charset=UTF-8\r\n";
  this->_responseFlag = true;
}

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