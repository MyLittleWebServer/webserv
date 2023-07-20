#include "RequestParser.hpp"

#include "CandidateFields.hpp"

RequestParser::RequestParser()
    : _candidateFields(CandidateFields::getInstance().getCandidateFields()) {}

RequestParser::~RequestParser() {}

RequestParser::RequestParser(const RequestParser &src)
    : _candidateFields(CandidateFields::getInstance().getCandidateFields()) {
  *this = src;
}

RequestParser &RequestParser::operator=(const RequestParser &src) {
  if (this != &src) {
  }
  return *this;
}

/**
 * @brief splitLinesByCRLF;
 *
 * CRLF에 따라 라인을 구분하여 dts->linesBuffer에 저장합니다.
 * CRLF가 연속된 이후는 dts->body에 저장합니다.
 *
 * @param RequestDts HTTP 관련 데이터
 *
 * @return void
 *
 * @author
 * @author middlefitting modify 2023.07.17
 * @date 2023.07.17
 */
void RequestParser::splitLinesByCRLF(RequestDts &dts) {
  size_t pos = 0;
  size_t delimeter = dts.request->find("\r\n");
  while (delimeter != std::string::npos) {
    std::string chunk = dts.request->substr(pos, delimeter - pos + 2);
    dts.linesBuffer->push_back(chunk);
    pos = delimeter + 2;
    delimeter = dts.request->find("\r\n", pos);
    if (delimeter == pos) {
      *dts.body = dts.request->substr(pos + 2);
      break;
    }
  }
}

void RequestParser::parseRequestLine(RequestDts &dts) {
  const std::string &firstLine(dts.linesBuffer->front());
  int delim_cnt = 0;
  size_t pos = firstLine.find(" ", 0);

  while (pos != std::string::npos) {
    delim_cnt++;
    pos = firstLine.find(" ", ++pos);
  }
  if (delim_cnt != 2) throw(*dts.statusCode = E_400_BAD_REQUEST);

  std::istringstream iss(firstLine);
  dts.linesBuffer->pop_front();
  iss >> *dts.method >> *dts.path >> *dts.protocol;
  size_t anchorPos = dts.path->find("#");
  if (anchorPos != std::string::npos) parseAnchor(dts, anchorPos);
  size_t qMarkPos = dts.path->find("?");
  if (qMarkPos != std::string::npos) parseQueryString(dts, qMarkPos);

  std::cout << "method: " << *dts.method << std::endl;
  std::cout << "path: " << *dts.path << std::endl;
  std::cout << "protocol: " << *dts.protocol << std::endl;
  if (*dts.method == "" || *dts.path == "" || *dts.protocol == "")
    throw(*dts.statusCode = E_400_BAD_REQUEST);
  checkRequestUriLimitLength(dts);
}

void RequestParser::parseAnchor(RequestDts &dts, size_t anchorPos) {
  *dts.anchor = &(*dts.path)[anchorPos + 1];
  *dts.path = dts.path->substr(0, anchorPos);
}

void RequestParser::parseQueryString(RequestDts &dts, size_t qMarkPos) {
  std::string tempQuery(
      dts.path->substr(qMarkPos + 1, dts.path->size() - qMarkPos - 1));
  *dts.query_string = tempQuery;
  *dts.path = dts.path->substr(0, qMarkPos);
  size_t andPos = tempQuery.find("&", 0);
  if (andPos == std::string::npos) {
    parseQueryKeyValue(dts, tempQuery);
    return;
  }
  size_t start = 0;
  while (andPos != std::string::npos) {
    std::string tempPair = tempQuery.substr(start, andPos);
    parseQueryKeyValue(dts, tempPair);
    start = andPos + 1;
    andPos = tempQuery.find("&", start);
  }
}

void RequestParser::parseQueryKeyValue(RequestDts &dts, std::string str) {
  size_t pos = str.find("=");
  if (pos == std::string::npos) throw(*dts.statusCode = E_400_BAD_REQUEST);
  std::string key = str.substr(0, pos);
  std::string value = str.substr(pos + 1, str.size() - pos - 1);
  (*dts.queryStringElements)[key] = value;
}

/**
 * @brief parseHeaderFields;
 *
 * HTTP 프로토콜은 \r\n\r\n 을 기준으로 헤더가 끝난 것을 판단할 수 있습니다.
 * 해당 함수에서는 해당 존재를 확인하여 헤더가 모두 들어왔는지 판단합니다.
 * 중복 금지 헤더를 체크합니다.
 *
 * @param RequestDts HTTP 관련 데이터
 *
 * @return void
 *
 * @author
 * @author middlefitting modify 2023.07.17
 * @date 2023.07.17
 */
void RequestParser::parseHeaderFields(RequestDts &dts) {
  std::list<std::string>::const_iterator lineIt = dts.linesBuffer->begin();
  std::list<std::string>::const_iterator lineEnd = dts.linesBuffer->end();

  std::string key;
  std::string value;
  size_t pos = 0;
  size_t end = 0;

  while (lineIt != lineEnd) {
    pos = (*lineIt).find(":");
    end = (*lineIt).find("\r\n");
    if (pos == std::string::npos || end == std::string::npos)
      throw(*dts.statusCode = E_400_BAD_REQUEST);
    key = std::string(toLowerString((*lineIt).substr(0, pos)));
    value =
        std::string(toLowerString((*lineIt).substr(pos + 1, end - pos - 1)));
    value = ft_trimOWS(value);
    validateHeaderKey(key, dts);
    removeNotAscii(key);
    removeNotAscii(value);
    if (_candidateFields.find(key) != _candidateFields.end()) {
      validateDuplicateInvalidHeaders(key, dts);
      (*dts.headerFields)[key] = value;
    }
    ++lineIt;
  }
  dts.linesBuffer->clear();
}

/**
 * @brief validateDuplicateInvalidHeaders;
 *
 * RFC 7230 3.3.2 Content-Length
 * Content-Length 헤더 필드가 중복될 경우 400 Bad Request를 반환합니다. (MUST)
 *
 * @param key 헤더의 키
 * @param RequestDts HTTP 관련 데이터
 *
 * @return void
 *
 * @author
 * @author middlefitting modify 2023.07.17
 * @date 2023.07.18
 */
void RequestParser::validateDuplicateInvalidHeaders(std::string key,
                                                    RequestDts &dts) {
  if (key == "content-length" || key == "host") {
    if (!(*dts.headerFields)[key].empty())
      throw(*dts.statusCode = E_400_BAD_REQUEST);
  }
}

void RequestParser::parseContent(RequestDts &dts) {
  if ((*dts.headerFields)["transfer-encoding"] == "" &&
      (*dts.headerFields)["content-length"] == "") {
    dts.body->clear();
    *dts.isParsed = true;
    return;
  } else if ((*dts.headerFields)["transfer-encoding"] != "")
    return parseTransferEncoding(dts);
  else
    return parseContentLength(dts);
}

void RequestParser::parseContentLength(RequestDts &dts) {
  *dts.contentLength =
      std::strtol((*dts.headerFields)["content-length"].c_str(), NULL, 10);
  if (dts.body->size() >= *dts.contentLength) {
    *dts.body = dts.body->substr(0, *dts.contentLength);
    *dts.isParsed = true;
  }
}

/**
 * @brief parseTransferEncoding;
 *
 * 클라이언트는 여러 인코딩을 사용하였을 경우 마지막에 chunked를 사용하여야
 * 합니다. (MUST) 따라서 마지막이 chunked가 아니면 400 에러를 반환합니다.
 * 구현되지 않은 인코딩에 대해서는 501 에러를 반환합니다.
 * 구현된 인코딩에 대해서는 파싱을 진행합니다.
 *
 * @param RequestDts HTTP 관련 데이터
 *
 * @return void
 *
 * @author
 * @author middlefitting modify 2023.07.17
 * @date 2023.07.17
 */
void RequestParser::parseTransferEncoding(RequestDts &dts) {
  std::vector<std::string> encodings =
      ft_split((*dts.headerFields)["transfer-encoding"], ',');
  size_t size = encodings.size();

  if (encodings[size - 1] != "chunked") {
    setConnectionClose(dts);
    throw(*dts.statusCode = E_400_BAD_REQUEST);
  }
  for (size_t i = 0; i < size; ++i) {
    encodings[i] = ft_trim(encodings[i]);
    if (encodings[i] == "chunked") {
      parseChunkedEncoding(dts);
      continue;
    }
    throw(*dts.statusCode = E_501_NOT_IMPLEMENTED);
  }
}

/**
 * @brief parseTransferEncoding;
 *
 * 특정 상황에서 connection 을 close 하기 위해 사용합니다.
 * connection 헤더 정보를 close로 설정합니다.
 *
 * @param RequestDts HTTP 관련 데이터
 *
 * @return void
 *
 * @author
 * @author middlefitting modify 2023.07.17
 * @date 2023.07.18
 */
void RequestParser::setConnectionClose(RequestDts &dts) {
  (*dts.headerFields)["connection"] = "close";
}

void RequestParser::parseChunkedEncoding(RequestDts &dts) {
  std::string body = *dts.body;
  dts.body->clear();
  *dts.contentLength = 0;
  size_t pos = 0;
  size_t end = 0;
  size_t chunkSize = 0;
  std::string chunk;

  while (pos != std::string::npos) {
    end = body.find("\r\n", pos);
    chunkSize = std::strtol(body.substr(pos, end - pos).c_str(), NULL, 16);
    if (chunkSize == 0) {
      *dts.isParsed = true;
      return;
    }
    chunk = body.substr(end + 2, chunkSize);
    *dts.body += chunk;
    *dts.contentLength += chunkSize;
    pos = end + 2 + chunkSize + 2;
  }
}

bool RequestParser::checkPathForm(RequestDts &dts) {
  std::string::const_iterator iter = dts.path->begin();
  std::string::const_iterator end = dts.path->end();
  if (*iter == '/') ++iter;
  while (iter != end) {
    if (*iter == '/' && *(iter + 1) == '/') return false;
    ++iter;
  }
  return true;
}

void RequestParser::setDefaultLocation(
    std::list<ILocationConfig *>::const_iterator defaultLocation,
    RequestDts &dts) {
  *dts.matchedLocation = *defaultLocation;
  dts.path->erase(0, 1);  // remove the first '/'
  *dts.path = (*defaultLocation)->getRoot() + *dts.path;
  dts.path->erase(0, 1);
}

void RequestParser::matchServerConf(short port, RequestDts &dts) {
  Config &config = Config::getInstance();
  std::list<IServerConfig *> serverInfo = config.getServerConfigs();
  std::list<IServerConfig *>::iterator it = serverInfo.begin();
  if (serverInfo.empty()) throw(42.42);
  while (it != serverInfo.end()) {
    if ((*it)->getListen() != port) {
      ++it;
      continue;
    }
    if ((*it)->getServerName() == (*dts.headerFields)["host"]) {
      *dts.matchedServer = *it;
      return;
    } else
      *dts.matchedServer = *it;
    ++it;
  }
  if (*dts.matchedServer == NULL) {
#ifdef DEBUG_MSG
    std::cout << "no matched server" << std::endl;
#endif
    throw(*dts.statusCode = E_404_NOT_FOUND);
  }
}

std::string RequestParser::getFirstTokenOfPath(RequestDts &dts) const {
  size_t pos = dts.path->find("/", 1);
  size_t end;
  if (pos == std::string::npos)
    end = dts.path->size();
  else
    end = pos;
  return (dts.path->substr(0, end));
}

// /root//dir/test.txt
// GET /dir/test.txt/ hTML/1.1
void RequestParser::validatePath(RequestDts &dts) {
  *dts.originalPath = *dts.path;  // for GET file list
  std::string firstToken = getFirstTokenOfPath(dts);
  if (firstToken == "/health") {
    throw(*dts.statusCode = E_200_OK);
  }
#ifdef DEBUG_MSG
  std::cout << "firstToken: " << firstToken << std::endl;
#endif
  std::list<ILocationConfig *>::const_iterator it =
      (*dts.matchedServer)->getLocationConfigs().begin();
  std::list<ILocationConfig *>::const_iterator endIt =
      (*dts.matchedServer)->getLocationConfigs().end();
  std::list<ILocationConfig *>::const_iterator defaultLocation;
  while (it != endIt) {
    const std::string &currRoute = (*it)->getRoute();
#ifdef DEBUG_MSG
    std::cout << "currRoute: " << currRoute << std::endl;
#endif
    if (currRoute == firstToken) {
      *dts.matchedLocation = *it;
      dts.path->erase(0, firstToken.size());
      checkAndParseRedirection(dts);
      if (dts.path->size() != 0 && (*dts.path)[0] == '/')
        dts.path->erase(0, 1);  // remove this because there is already a
                                // slash at the end of root path
      *dts.path = (*it)->getRoot() + *dts.path;
      dts.path->erase(0, 1);  // remove the first '/'
#ifdef DEBUG_MSG
      std::cout << "actual path: " << *dts.path << '\n';
#endif
      if (this->checkPathForm(dts) == false)
        throw(*dts.statusCode = E_404_NOT_FOUND);
      return;
    }
    if (currRoute == "/") defaultLocation = it;
    ++it;
  }
  this->setDefaultLocation(defaultLocation, dts);
}

void RequestParser::checkAndParseRedirection(RequestDts &dts) {
  unsigned short value;
  try {
    std::stringstream ss((*dts.matchedLocation)->getVariable("return"));
    std::string redirectLocation;
    ss >> value >> redirectLocation;
    if (redirectLocation != "/") *dts.path = redirectLocation + *dts.path;
  } catch (ExceptionThrower::InvalidConfigException &e) {
    return;
  }
  switch (value) {
    case 301:
      throw(*dts.statusCode = E_301_MOVED_PERMANENTLY);
    case 302:
      throw(*dts.statusCode = E_302_FOUND);
    case 303:
      throw(*dts.statusCode = E_303_SEE_OTHER);
    case 304:
      throw(*dts.statusCode = E_304_NOT_MODIFIED);
    case 307:
      throw(*dts.statusCode = E_307_TEMPORARY_REDIRECT);
    case 308:
      throw(*dts.statusCode = E_308_PERMANENT_REDIRECT);
  }
}

void RequestParser::parseCgi(RequestDts &dts) {
  *dts.is_cgi = false;
  const std::string &extension = (*dts.matchedServer)->getCgi();
  if (dts.path->size() < extension.size()) return;
  std::string cgiPath =
      dts.path->substr(dts.path->size() - extension.size(), extension.size());
  if (cgiPath != extension) return;
  *dts.is_cgi = true;
  *dts.cgi_path = cgiPath;
}

/**
 * @brief validateHeaderKey;
 *
 * 헤더 키 값에 공백이 존재하면 400 에러를 발생시킵니다.
 *
 * @param key 헤더 키 값
 * @param RequestDts HTTP 관련 데이터
 *
 * @return void
 *
 * @author middlefitting
 * @date 2023.07.17
 */
void RequestParser::validateHeaderKey(std::string &key, RequestDts &dts) {
  std::string::size_type pos = 0;
  while (pos < key.length() && !std::isspace(key[pos])) ++pos;
  if (pos != key.length()) throw(*dts.statusCode = E_400_BAD_REQUEST);
}

/**
 * @brief removeNotAscii;
 *
 * 헤더 필드 값에 ASCII가 아닌 값이 존재하면 제거합니다.
 *
 * @param field 헤더 필드 값
 *
 * @return void
 *
 * @author middlefitting
 * @date 2023.07.17
 */
void RequestParser::removeNotAscii(std::string &field) {
  while (true) {
    const char *tmp = field.c_str();
    int len = field.length();
    if (len == 0) return;
    for (int i = 0; i < len; i++) {
      if (tmp[i] < 0 || tmp[i] > 127) {
        field.erase(i, 1);
        break;
      }
      if (i == len - 1) return;
    }
  }
}

/**
 * @brief allHeaderRecieved;
 *
 * HTTP 프로토콜은 \r\n\r\n 을 기준으로 헤더가 끝난 것을 판단할 수 있습니다.
 * 해당 함수에서는 해당 존재를 확인하여 헤더가 모두 들어왔는지 판단합니다.
 *
 * @param RequestDts HTTP 관련 데이터
 *
 * @return request에 \r\n\r\n 존재여부를 반환합니다.
 *
 * @author middlefitting
 * @date 2023.07.16
 */
bool RequestParser::allHeaderRecieved(RequestDts &dts) {
  size_t ret = dts.request->find("\r\n\r\n");
  if (ret == std::string::npos) return false;
  return true;
}

/**
 * @brief parseRequest;
 *
 * request 파싱을 위한 전체 순서를 제어합니다.
 *
 * @param RequestDts HTTP 관련 데이터
 * @param port 서버 포트 번호
 *
 * @return void
 *
 * @author middlefitting
 * @date 2023.07.18
 */
void RequestParser::parseRequest(RequestDts &dts, short port) {
  if (!allHeaderRecieved(dts)) {
    attackGuard(dts);
    return;
  }
  splitLinesByCRLF(dts);
  parseRequestLine(dts);
  parseHeaderFields(dts);
  validateContentLengthHeader(dts);
  validateHostHeader(port, dts);
  parseContent(dts);
  matchServerConf(port, dts);
  validatePath(dts);
  parseCgi(dts);
  requestChecker(dts);
}

/**
 * @brief attackGuard;
 *
 * CRLF CRLF가 들어오지 않은 상황에서 클라이언트가 악의적으로 긴 데이터를 보내는
 * 것을 방지합니다.
 *
 * @param RequestDts HTTP 관련 데이터
 *
 * @return request에 \r\n\r\n 존재여부를 반환합니다.
 *
 * @author middlefitting
 * @date 2023.07.18
 */
void RequestParser::attackGuard(RequestDts &dts) {
  IProxyConfig &proxyConfig = Config::getInstance().getProxyConfig();
  if (dts.request->size() > (proxyConfig.getClientMaxBodySize() +
                             proxyConfig.getRequestHeaderLimitSize() +
                             proxyConfig.getRequestUriLimitSize()))
    throw(*dts.statusCode = E_413_REQUEST_ENTITY_TOO_LARGE);
}

RequestParser &RequestParser::getInstance() {
  static RequestParser instance;
  return instance;
}

void RequestParser::requestChecker(RequestDts &dts) {
  checkContentRangeHeader(dts);
  checkRequestLine(dts);
  checkContentLenghWithTransferEncoding(dts);
  checkHeaderLimitSize(dts);
  checkBodyLimitLength(dts);
  checkAllowedMethods(dts);
  checkCgiMethod(dts);
  checkTE(dts);
}

/**
 * @brief validateHostHeader;
 *
 * RFC 7230 5.4 Host
 * Host 헤더가 없다면 400 에러를 발생시킵니다.
 * Host 헤더에 포트가 존재하면 호스트명과 포트,
 * 존재하지 않는다면 호스트명만 체크합니다.
 *
 * @param RequestDts HTTP 요청 데이터.
 * @return void
 * @author middlefitting
 * @date 2023.07.19
 */
void RequestParser::validateHostHeader(short port, RequestDts &dts) {
  if ((*dts.headerFields)["host"].empty())
    throw(*dts.statusCode = E_400_BAD_REQUEST);
  std::string hostHeader = (*dts.headerFields)["host"];
  std::string::size_type pos = hostHeader.find(':');
  std::string hostName;
  if (pos == std::string::npos) {
    hostName = hostHeader;
    hostHeaderNameCheck(hostName, dts);
  } else {
    hostName = hostHeader.substr(0, pos);
    hostHeaderNameCheck(hostName, dts);
    hostHeaderportCheck(port, hostHeader.substr(pos + 1), dts);
  }
  (*dts.headerFields)["host"] = hostName;
}

/**
 * @brief hostHeaderNameCheck;
 *
 * 호스트명이 올바른 형식인지 체크합니다.
 * 호스트 명은 클라이언트가 알지 못해 비어서 오는 경우,
 * 영문, 숫자, ., - 만 허용합니다.
 * 올바르지 않다면 400 에러를 발생시킵니다.
 *
 * @param RequestDts HTTP 요청 데이터.
 * @return void
 * @author middlefitting
 * @date 2023.07.19
 */
void RequestParser::hostHeaderNameCheck(std::string hostHeader,
                                        RequestDts &dts) {
  if (ft_trim(hostHeader).empty()) return;
  for (int i = 0; i < static_cast<int>(hostHeader.size()); i++) {
    if (!std::isalnum(hostHeader[i]) && hostHeader[i] != '.' &&
        hostHeader[i] != '-') {
      throw(*dts.statusCode = E_400_BAD_REQUEST);
    }
  }
}

/**
 * @brief hostHeaderportCheck;
 *
 * 호스트 포트가 올바른 형식인지 체크합니다.
 * 포트는 0 ~ 65535 사이의 숫자만 허용합니다.
 * 숫자의 첫자리가 0인 경우를 허용하지 않습니다.
 * 0번 포트는 일반적으로 예약된 포트로 사용하지 않습니다.
 * 올바르지 않다면 400 에러를 발생시킵니다.
 *
 * @param RequestDts HTTP 요청 데이터.
 * @return void
 * @author middlefitting
 * @date 2023.07.19
 */
void RequestParser::hostHeaderportCheck(short port, std::string portName,
                                        RequestDts &dts) {
  if (portName.empty()) throw(*dts.statusCode = E_400_BAD_REQUEST);
  if (portName.find_first_not_of("0123456789") != std::string::npos)
    throw(*dts.statusCode = E_400_BAD_REQUEST);
  if (portName.find_first_of("123456789") != 0)
    throw(*dts.statusCode = E_400_BAD_REQUEST);
  if (portName.size() > 5) throw(*dts.statusCode = E_400_BAD_REQUEST);
  if (std::atoi(portName.c_str()) != port)
    throw(*dts.statusCode = E_400_BAD_REQUEST);
  if (std::atoi(portName.c_str()) > 65535)
    throw(*dts.statusCode = E_400_BAD_REQUEST);
}

/**
 * @brief validateContentLengthHeader;
 *
 * RFC 7230 3.3.2 Content-Length
 * Content-Length 헤더 필드의 value가 유효한지 검증합니다.
 * 해당 헤더가 없다면 검증하지 않습니다.
 * value가 숫자가 아니거나, 0이 아닌데 0으로 시작한다면 400 에러를
 * 발생시킵니다. overflow 방지 차원에서 10자리 이상의 숫자는 413 에러를
 * 발생시킵니다.
 *
 * @param RequestDts HTTP 요청 데이터.
 * @return void
 * @author middlefitting
 * @date 2023.07.18
 */
void RequestParser::validateContentLengthHeader(RequestDts &dts) {
  std::string content_length = (*dts.headerFields)["content-length"];
  if (content_length.empty()) return;
  if (content_length.find_first_not_of("0123456789") != std::string::npos)
    throw(*dts.statusCode = E_400_BAD_REQUEST);
  if (content_length.find_first_not_of("0") != std::string::npos &&
      content_length.find_first_not_of("0") != 0)
    throw(*dts.statusCode = E_400_BAD_REQUEST);
  if (content_length.size() >= 10)
    throw(*dts.statusCode = E_413_REQUEST_ENTITY_TOO_LARGE);
}

/**
 * @brief checkRequestLine;
 *
 * RFC 7230 3.1.1 Request Line
 *
 * @param RequestDts HTTP 요청 데이터.
 * @return void
 * @author middlefitting
 * @date 2023.07.17
 */
void RequestParser::checkRequestLine(RequestDts &dts) {
  checkMethod(dts);
  checkProtocolVersion(dts);
}

/**
 * @brief checkMethod;
 *
 * 구현하지 못한 메서드를 수신하면 501(Not Implemented) 응답 (SHOULD)
 *
 * @param RequestDts HTTP 관련 데이터
 * @return void
 * @author middlefitting
 * @date 2023.07.17
 */
void RequestParser::checkMethod(RequestDts &dts) {
  if (*dts.method != "GET" && *dts.method != "POST" &&
      *dts.method != "DELETE" && *dts.method != "HEAD" &&
      *dts.method != "OPTIONS")
    throw(*dts.statusCode = E_501_NOT_IMPLEMENTED);
}

/**
 * @brief checkProtocolVersion;
 *
 * 유효하지 않은 request-line을 수신하면 400(Bad Request) 응답 (SHOULD)
 * 제공하지 못하는 프로토콜 버전에 대해서는 505(HTTP Version Not Supported) 응답
 *
 * @param RequestDts HTTP 관련 데이터
 * @return void
 * @author
 * @date 2023.07.17
 */
void RequestParser::checkProtocolVersion(RequestDts &dts) {
  const std::string &protocol = *dts.protocol;
  if (protocol.substr(0, 5) != "HTTP/" || protocol.size() != 8)
    throw(*dts.statusCode = E_400_BAD_REQUEST);
  const std::string versionStr = protocol.substr(5, 3);
  if (versionStr == "1.1" || versionStr == "1.0") return;
  if (versionStr.find(".") != 1) throw(*dts.statusCode = E_400_BAD_REQUEST);
  char *endPtr;
  double versionVal = std::strtod(versionStr.c_str(), &endPtr);
  if (*endPtr != '\0') throw(*dts.statusCode = E_400_BAD_REQUEST);
  if (versionVal > 1.1 || versionVal < 1.0) {
    throw(*dts.statusCode = E_505_HTTP_VERSION_NOT_SUPPORTED);
  }
}

/**
 * @brief checkRequestUriLimitLength;
 *
 * request-target이 서버 URI보다 길면 414(URI Too Long) (MUST)
 *
 * @param RequestDts HTTP 관련 데이터
 * @return void
 * @author middlefitting
 * @date 2023.07.17
 */
void RequestParser::checkRequestUriLimitLength(RequestDts &dts) {
  if (dts.path->size() >
      Config::getInstance().getProxyConfig().getRequestUriLimitSize())
    throw(*dts.statusCode = E_414_URI_TOO_LONG);
}

/**
 * @brief checkContentLenghWithTransferEncoding;
 *
 * 발신자는 Transfer-Encoding 과 Content-Length 같이 보내면 안 된다.(MUST NOT)
 * Transfer-Encoding 과 Content-Length이 둘 다 있으면 400 에러를 반환합니다.
 *
 * @param RequestDts HTTP 관련 데이터
 * @return void
 * @author middlefitting
 * @date 2023.07.17
 */
void RequestParser::checkContentLenghWithTransferEncoding(RequestDts &dts) {
  if ((*dts.headerFields)["content-length"] != "" &&
      (*dts.headerFields)["transfer-encoding"] != "")
    throw(*dts.statusCode = E_400_BAD_REQUEST);
}

/**
 * @brief checkHeaderLimitSize;
 *
 * 헤더 길이가 서버 기준치를 넘으면 413(Request Entity Too Large) (MUST)
 * Body가 등장하기 이전까지의 길이를 기준으로 합니다.
 *
 * @param RequestDts HTTP 관련 데이터
 * @return void
 * @author middlefitting
 * @date 2023.07.17
 */
void RequestParser::checkHeaderLimitSize(RequestDts &dts) {
  size_t pos = dts.request->find("\r\n\r\n");
  if (pos == std::string::npos) return;
  if (pos > Config::getInstance().getProxyConfig().getRequestHeaderLimitSize())
    throw(*dts.statusCode = E_413_REQUEST_ENTITY_TOO_LARGE);
}

void RequestParser::checkBodyLimitLength(RequestDts &dts) {
  if (*dts.contentLength > ((*dts.matchedLocation)->getLimitClientBodySize()))
    throw(*dts.statusCode = E_413_REQUEST_ENTITY_TOO_LARGE);
}

void RequestParser::checkAllowedMethods(RequestDts &dts) {
  const std::map<std::string, bool> &methodInfo =
      (*dts.matchedLocation)->getAllowMethod();
  std::map<std::string, bool>::const_iterator it = methodInfo.find(*dts.method);
  if ((it != methodInfo.end() && it->second == true)) return;
  throw(*dts.statusCode = E_405_METHOD_NOT_ALLOWED);
}

void RequestParser::checkCgiMethod(RequestDts &dts) {
  if (*dts.is_cgi && *dts.method != "GET" && *dts.method != "POST")
    throw(*dts.statusCode = E_400_BAD_REQUEST);
}

/**
 * @brief checkTE;
 *
 * chunked는 항상 구현되어야 하기 때문에 TE 헤더에서 허용되지 않습니다.
 * TE 헤더에 chunked가 있으면 400 에러를 반환합니다.
 * 구현되지 않은 인코딩에 대해 501 에러를 반환합니다.
 *
 * @param RequestDts HTTP 관련 데이터
 * @return void
 * @author middlefitting
 * @date 2023.07.17
 */
void RequestParser::checkTE(RequestDts &dts) {
  if (ft_trim((*dts.headerFields)["te"]).empty()) return;
  if ((*dts.headerFields)["te"] == "chunked")
    throw(*dts.statusCode = E_400_BAD_REQUEST);
  throw(*dts.statusCode = E_501_NOT_IMPLEMENTED);
}

/**
 * @brief checkContentRangeHeader;
 *
 * RFC 7231 4.3.4 PUT MUST
 * Content-Range가 PUT 요청에 사용되었을 때 400 응답을 보냅니다.
 *
 * @param RequestDts HTTP 관련 데이터
 * @return void
 * @author middlefitting
 * @date 2023.07.21
 */
void RequestParser::checkContentRangeHeader(RequestDts &dts) {
  if (ft_trim((*dts.headerFields)["content-range"]).empty()) return;
  if (*dts.method == "PUT") throw(*dts.statusCode = E_400_BAD_REQUEST);
}
