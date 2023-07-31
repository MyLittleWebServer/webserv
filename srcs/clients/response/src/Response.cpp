#include "Response.hpp"

#include "Status.hpp"

Response::Response()
    : _responseFlag(false), _assembleFlag(false), _statusCode(E_200_OK) {}

Response::~Response() {}

Response::Response(const Response &src) { *this = src; }

Response &Response::operator=(const Response &src) {
  if (this != &src) {
    _response = src._response;
    _responseFlag = src._responseFlag;
    _assembleFlag = src._assembleFlag;
    _statusCode = src._statusCode;
    _body = src._body;
  }
  return (*this);
}

const std::string &Response::getResponse(void) const { return (_response); }

const std::string &Response::getBody(void) const { return (_body); }

bool Response::getResponseFlag(void) const { return (_responseFlag); }

Status Response::getStatus(void) { return (_statusCode); }

std::string &Response::getFieldValue(const std::string &key) {
  return (_headerFields[key]);
}

/**
 * @brief 300번대 응답을 생성합니다.
 *
 * @details
 * 301, 302, 303, 304, 307, 308번대 응답을 생성합니다.
 *
 * @see E_301_MOVED_PERMANENTLY
 * @see E_302_FOUND
 * @see E_303_SEE_OTHER
 * @see E_304_NOT_MODIFIED
 * @see E_307_TEMPORARY_REDIRECT
 * @see E_308_PERMANENT_REDIRECT
 *
 * dts.path가 비어있으면 "/"로 설정합니다.
 * query_string이 비어있지 않으면 path에 query_string을 붙여준다.
 *
 * @param dts
 *
 * @author tocatoca
 * @date 2023.07.29
 */
void Response::create300Response(RequestDts &dts) {
  if (dts.path->empty()) {
    *dts.path = "/";
  }
  if (!(*dts.query_string).empty()) {
    *dts.path += '?' + *dts.query_string;
  }
  setHeaderField("Location", *dts.path);
  assembleResponse();
}

/**
 * @brief 400, 500번대 응답을 생성합니다.
 *
 * @details
 *
 * @see E_400_BAD_REQUEST
 * @see E_401_UNAUTHORIZED
 * @see E_403_FORBIDDEN
 * @see E_404_NOT_FOUND
 * @see E_405_METHOD_NOT_ALLOWED
 * @see E_406_NOT_ACCEPTABLE
 * @see E_408_REQUEST_TIMEOUT
 * @see E_409_CONFLICT
 * @see E_413_REQUEST_ENTITY_TOO_LARGE
 * @see E_414_URI_TOO_LONG
 * @see E_415_UNSUPPORTED_MEDIA_TYPE
 * @see E_417_EXPECTION_FAILED
 * @see E_418_IM_A_TEAPOT
 * @see E_500_INTERNAL_SERVER_ERROR
 * @see E_501_NOT_IMPLEMENTED
 * @see E_502_BAD_GATEWAY
 * @see E_503_SERVICE_UNAVAILABLE
 * @see E_504_GATEWAY_TIMEOUT
 * @see E_505_HTTP_VERSION_NOT_SUPPORTE
 *
 * @param dts
 *
 * @author tocatoca
 * @date 2023.07.29
 */
void Response::create400And500Response(RequestDts &dts) {
  if (*dts.matchedServer != NULL) {
    configureErrorPages(dts);
  } else {
    addBody(statusInfo[*dts.statusCode].body);
    addBody("\r\n");
  }
  setHeaderField("Content-Length", itos(_body.size()));
  assembleResponse();
}

/**
 * @brief 예외 응답을 생성합니다.
 *
 * @details
 * 각각의 status code에 맞춰서 response를 생성합니다.
 * 생성후 responseFlag를 on하여 response의 생성을 알립니다.
 *
 * @param dts
 */
void Response::createExceptionResponse(RequestDts &dts) {
  resetResponse();
  _statusCode = *dts.statusCode;
  if (_statusCode >= E_301_MOVED_PERMANENTLY &&
      _statusCode <= E_308_PERMANENT_REDIRECT) {
    create300Response(dts);
  } else {
    create400And500Response(dts);
  }
  _responseFlag = true;
}

/**
 * @brief 비어있는 예외 응답을 생성합니다.
 *
 * @details
 * 400번대 status code들을 조건으로 response를 생성합니다.
 * 생성후 responseFlag를 on하여 response의 생성을 알립니다.
 *
 * @param dts
 */
void Response::createEmptyExceptionResponse(RequestDts &dts) {
  resetResponse();
  _statusCode = *dts.statusCode;
  if (_statusCode > E_308_PERMANENT_REDIRECT) create400And500Response(dts);
  _responseFlag = true;
}

/**
 * @brief 응답을 초기화합니다.
 *
 * @details
 * _responseFlag가 false면 리턴하여 response clear를 진행하지 않는다.
 *
 * @author tocatoca
 * @date 2023.07.29
 */
void Response::resetResponse(void) {
  if (_responseFlag == false) return;
  _response.clear();
  _responseFlag = false;
}

/**
 * @brief 응답을 파싱했음을 설정합니다.
 *
 * @details
 * _assembleFlag를 true로 설정합니다.
 * _responseFlag를 true로 설정합니다.
 *
 * @author tocatoca
 * @date 2023.07.29
 */
void Response::setResponseParsed() {
  _assembleFlag = true;
  _responseFlag = true;
}
bool Response::isParsed() { return _responseFlag; }

/**
 * @brief 응답을 조립합니다.
 *
 * @details
 * assembleResponseLine을 호출합니다.
 * putHeaderFields를 호출합니다.
 * putBody를 호출합니다.
 *
 * @author tocatoca
 * @date 2023.07.29
 */
void Response::assembleResponse(void) {
  assembleResponseLine();
  putHeaderFields();
  putBody();
}

/**
 * @brief 응답 라인을 조립합니다.
 *
 * @details
 * HTTP/1.1 200 OK\r\n 와 같이 조립해야합니다.
 *
 * _statusCode를 이용하여 응답 라인을 조립합니다.
 *
 * @see _statusCode.code : 응답 코드
 * @see _statusCode.message : 응답 메시지
 * @see statusInfo : 응답 코드와 메시지를 저장한 구조체
 *
 * @author tocatoca
 * @date 2023.07.29
 */
void Response::assembleResponseLine(void) {
  _response = "HTTP/1.1 ";
  _response += statusInfo[_statusCode].code;
  _response += " ";
  _response += statusInfo[_statusCode].message;
  _response += "\r\n";
}

/**
 * @brief 헤더 필드를 조립합니다.
 *
 * @details
 * _headerFields를 순회하며 헤더 필드를 조립합니다.
 *
 * @see _headerFields : 헤더 필드를 저장한 맵
 * @see _response : 조립된 응답
 *
 * @author tocatoca
 * @date 2023.07.29
 */
void Response::putHeaderFields(void) {
  std::map<std::string, std::string>::const_iterator it = _headerFields.begin();
  std::map<std::string, std::string>::const_iterator end = _headerFields.end();

  for (; it != end; ++it) {
    _response += it->first + ": " + it->second + "\r\n";
  }
}

/**
 * @brief Body를 추가합니다.
 *
 * @author tocatoca
 * @date 2023.07.29
 */
void Response::putBody(void) {
  if (_body.empty() == true) {
    _response += "\r\n";
    return;
  }
  _response += "\r\n" + _body;
}

void Response::setStatusCode(Status code) { _statusCode = code; }

/**
 * @brief 헤더 필드를 설정합니다.
 *
 * @param key
 * @param value
 */
void Response::setHeaderField(const std::string &key,
                              const std::string &value) {
  _headerFields[key] = value;
}

void Response::eraseHeaderField(const std::string &key) {
  _headerFields.erase(key);
}

void Response::addBody(const std::string &str) { _body += str; }

void Response::setBody(const std::string &str) { _body = str; }

/**
 * @brief 에러페이지를 구성합니다.
 *
 * @details
 * config file에서 serverblock의 error_page를 이용하여 에러페이지를 구성합니다.
 * 3000 port에서는 에러페이지를 directory로 사용하여 구성합니다.
 * 8080 port는 에러페이지를 file로 사용하여 구성합니다.
 *
 * @param dts
 */
void Response::configureErrorPages(RequestDts &dts) {
  IServerConfig &serverConfig = **dts.matchedServer;

  std::string path = serverConfig.getErrorPage();
  if (path[path.size() - 1] == '/') {
    path += statusInfo[*dts.statusCode].code;
    path += ".html";
  }

  if (path[0] == '/') {
    path = path.substr(1);
  }
  std::ifstream file(path.c_str(), std::ios::in);
  if (file.is_open() == false) {
    setHeaderField("Content-Type", "text/plain");
    addBody(statusInfo[*dts.statusCode].body);
    addBody("\r\n");
    return;
  } else {
    setHeaderField("Content-Type", "text/html; charset=UTF-8");
    std::string buff;
    while (std::getline(file, buff)) {
      addBody(buff);
      addBody("\r\n");
    }
    file.close();
  }
}

/**
 * @brief 맛있는 쿠키를 구워줍니다.
 *
 * @details
 * httponly, Max-Age=60; 옵션을 추가합니다.
 *
 * @see httpOnly : 자바스크립트에서 쿠키를 읽을 수 없도록 합니다.
 * @see Max-Age : 쿠키의 유효기간을 설정합니다. (초 단위)
 *
 * @param session_id
 */
void Response::setCookie(std::string &session_id) {
  setHeaderField("Set-Cookie",
                 "session_id=" + session_id + "; httponly; Max-Age=60;");
}

void Response::clear() {
  _response.clear();
  _responseFlag = false;
  _assembleFlag = false;
  _statusCode = E_200_OK;
  _body.clear();
  _headerFields.clear();
}
