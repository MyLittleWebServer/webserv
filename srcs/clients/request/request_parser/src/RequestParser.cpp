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

void RequestParser::splitLinesByCRLF(RequestDts &dts) {
  size_t pos = 0;
  size_t delimeter = dts.request->find("\r\n");
  while (delimeter != std::string::npos) {
    std::string chunk = dts.request->substr(pos, delimeter);
    dts.linesBuffer->push_back(chunk);
    pos = delimeter + 2;
    delimeter = dts.request->find("\r\n", pos);
    if (delimeter == pos) {
      *dts.body = &(*dts.request)[pos + 2];
      break;
    }
  }
  // 할지 안할지 고려
  // dts.request->clear();
}

void RequestParser::parseRequestLine(RequestDts &dts) {
  const std::string &firstLine(dts.linesBuffer->front());
  int delim_cnt = 0;
  size_t pos = firstLine.find(" ", 0);
  while (pos != std::string::npos) {
    delim_cnt++;
    pos = firstLine.find(" ", ++pos);
  }
  if (delim_cnt != 2) throw(*dts.statusCode = BAD_REQUEST);

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
    throw(*dts.statusCode = BAD_REQUEST);
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
  if (pos == std::string::npos) throw(*dts.statusCode = BAD_REQUEST);
  std::string key = str.substr(0, pos);
  std::string value = str.substr(pos + 1, str.size() - pos - 1);
  (*dts.queryStringElements)[key] = value;
}

void RequestParser::parseHeaderFields(RequestDts &dts) {
  std::list<std::string>::const_iterator lineIt = dts.linesBuffer->begin();
  std::list<std::string>::const_iterator lineEnd = dts.linesBuffer->end();

  std::string key;
  std::string value;
  size_t pos = 0;
  size_t end = 0;

  while (lineIt != lineEnd) {
    pos = (*lineIt).find(": ");
    end = (*lineIt).find("\r\n");
    key = toLowerString((*lineIt).substr(0, pos));
    if (_candidateFields.find(key) != _candidateFields.end()) {
      value = toLowerString((*lineIt).substr(pos + 2, end - pos - 2));
      // value 검증 필요
      (*dts.headerFields)[key] = value;
    }
    ++lineIt;
  }
  dts.linesBuffer->clear();
}

void RequestParser::parseContent(RequestDts &dts) {
  if (dts.body->empty())
    return;
  else if ((*dts.headerFields)["transfer-encoding"] != "")
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

void RequestParser::parseTransferEncoding(RequestDts &dts) {
  if ((*dts.headerFields)["transfer-encoding"] == "chunked")
    return parseChunkedEncoding(dts);
}

void RequestParser::parseChunkedEncoding(RequestDts &dts) {
  std::string body = *dts.body;
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
  dts.matchedLocation = *defaultLocation;
  dts.path->erase(0, 1);  // remove the first '/'
  *dts.path = (*defaultLocation)->getRoot() + *dts.path;
  dts.path->erase(0, 1);
}

void RequestParser::matchServerConf(short port, RequestDts &dts) {
  dts.matchedServer = NULL;
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
      dts.matchedServer = *it;
      return;
    } else
      dts.matchedServer = *it;
    ++it;
  }
  if (dts.matchedServer == NULL) {
#ifdef DEBUG_MSG
    std::cout << "no matched server" << std::endl;
#endif
    throw(*dts.statusCode = NOT_FOUND);
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
  std::string firstToken = getFirstTokenOfPath(dts);
#ifdef DEBUG_MSG
  std::cout << "firstToken: " << firstToken << std::endl;
#endif
  std::list<ILocationConfig *>::const_iterator it =
      dts.matchedServer->getLocationConfigs().begin();
  std::list<ILocationConfig *>::const_iterator endIt =
      dts.matchedServer->getLocationConfigs().end();
  std::list<ILocationConfig *>::const_iterator defaultLocation;
  while (it != endIt) {
    const std::string &currRoute = (*it)->getRoute();
#ifdef DEBUG_MSG
    std::cout << "currRoute: " << currRoute << std::endl;
#endif
    if (currRoute == firstToken) {
      dts.matchedLocation = *it;
      dts.path->erase(0, firstToken.size());
      if (dts.path->size() != 0 && (*dts.path)[0] == '/')
        dts.path->erase(0, 1);  // remove this because there is already a
                                // slash at the end of root path
      *dts.path = (*it)->getRoot() + *dts.path;
      dts.path->erase(0, 1);  // remove the first '/'
#ifdef DEBUG_MSG
      std::cout << "actual path: " << *dts.path << '\n';
#endif
      if (this->checkPathForm(dts) == false) throw(*dts.statusCode = NOT_FOUND);
      return;
    }
    if (currRoute == "/") defaultLocation = it;
    ++it;
  }
  this->setDefaultLocation(defaultLocation, dts);
}

void RequestParser::parseCgi(RequestDts &dts) {
  *dts.is_cgi = false;
  const std::string &extension = dts.matchedServer->getCgi();
  if (dts.path->size() < extension.size()) return;
  std::string cgiPath =
      dts.path->substr(dts.path->size() - extension.size(), extension.size());
  if (cgiPath != extension) return;
  *dts.is_cgi = true;
  *dts.cgi_path = cgiPath;
}

void RequestParser::parseRequest(RequestDts &dts, short port) {
  splitLinesByCRLF(dts);
  parseRequestLine(dts);
  parseHeaderFields(dts);
  parseContent(dts);
  matchServerConf(port, dts);
  validatePath(dts);
  parseCgi(dts);
  requestChecker(dts);
}

RequestParser &RequestParser::getInstance() {
  static RequestParser instance;
  return instance;
}

void RequestParser::requestChecker(RequestDts &dts) {
  checkContentLenghWithTransferEncoding(dts);
  checkRequestUriLimitLength(dts);
  checkHeaderLimitSize(dts);
  checkBodyLimitLength(dts);
  checkAllowedMethods(dts);
  checkCgiMethod(dts);
  if (dts.isParsed == false) return;
}

void RequestParser::checkContentLenghWithTransferEncoding(RequestDts &dts) {
  if ((*dts.headerFields)["content-length"] != "" &&
      (*dts.headerFields)["transfer-encoding"] != "")
    throw(*dts.statusCode = BAD_REQUEST);
}

void RequestParser::checkRequestUriLimitLength(RequestDts &dts) {
  // body uri header별로 따로따로 정의할건지 정하기
  if (dts.path->size() >
      Config::getInstance().getProxyConfig().getRequestUriLimitSize())
    throw(*dts.statusCode = REQUEST_ENTITY_TOO_LARGE);
}

void RequestParser::checkHeaderLimitSize(RequestDts &dts) {
  std::map<std::string, std::string>::const_iterator lineIt =
      dts.headerFields->begin();
  std::map<std::string, std::string>::const_iterator lineEnd =
      dts.headerFields->end();

  while (lineIt != lineEnd) {
    if (lineIt->first.size() >
        Config::getInstance().getProxyConfig().getRequestHeaderLimitSize())
      throw(*dts.statusCode = REQUEST_ENTITY_TOO_LARGE);
    ++lineIt;
  }
}

void RequestParser::checkBodyLimitLength(RequestDts &dts) {
  if (*dts.contentLength > dts.matchedLocation->getLimitClientBodySize())
    throw(*dts.statusCode = REQUEST_ENTITY_TOO_LARGE);
}

void RequestParser::checkAllowedMethods(RequestDts &dts) {
  const std::map<std::string, bool> &method_info =
      dts.matchedLocation->getAllowMethod();
  std::map<std::string, bool>::const_iterator it =
      method_info.find(*dts.method);
  if (it != method_info.end() && it->second == true) return;
  throw(*dts.statusCode = METHOD_NOT_ALLOWED);
}

void RequestParser::checkCgiMethod(RequestDts &dts) {
  if (*dts.is_cgi && *dts.method != "GET" && *dts.method != "POST")
    throw(*dts.statusCode = BAD_REQUEST);
}
