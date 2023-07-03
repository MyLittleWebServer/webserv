#include "RequestParser.hpp"

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
  dts.request->clear();
}

void RequestParser::parseRequestLine(RequestDts &dts) {
  const std::string &firstLine(dts.linesBuffer->front());
  int delim_cnt = 0;
  size_t pos = firstLine.find(" ", 0);
  while (pos != std::string::npos) {
    delim_cnt++;
    pos = firstLine.find(" ", ++pos);
  }
  if (delim_cnt != 2) throw(_statusCode = BAD_REQUEST);

  std::istringstream iss(firstLine);
  dts.linesBuffer->pop_front();
  iss >> *dts.method >> *dts.path >> *dts.protocol;

  size_t anchorPos = dts.path->find("#");
  if (anchorPos != std::string::npos) parseAnchor(dts, anchorPos);
  size_t qMarkPos = dts.path->find("?");
  if (qMarkPos != std::string::npos) parseQueryString(dts, qMarkPos);

  std::cout << "method: " << dts.method << std::endl;
  std::cout << "path: " << dts.path << std::endl;
  std::cout << "protocol: " << dts.protocol << std::endl;
  if (*dts.method == "" || *dts.path == "" || *dts.protocol == "")
    throw(_statusCode = BAD_REQUEST);
}

void RequestParser::parseAnchor(RequestDts &dts, size_t anchorPos) {
  *dts.anchor = &(*dts.path)[anchorPos + 1];
  *dts.path = dts.path->substr(0, anchorPos);
}

void RequestParser::parseQueryString(RequestDts &dts, size_t qMarkPos) {
  std::string tempQuery(
      dts.path->substr(qMarkPos + 1, dts.path->size() - qMarkPos - 1));
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
  if (pos == std::string::npos) throw(_statusCode = BAD_REQUEST);
  std::string key = str.substr(0, pos);
  std::string value = str.substr(pos + 1, str.size() - pos - 1);
  (*dts.queryString)[key] = value;
}

void RequestParser::parseHeaderFields(RequestDts &dts) {
  std::list<std::string>::const_iterator lineIt = dts.linesBuffer->begin();
  std::list<std::string>::const_iterator lineEnd = dts.linesBuffer->end();

  std::string key;
  std::string value;

  size_t pos = 0;
  while (lineIt != lineEnd) {
    pos = (*lineIt).find(": ");
    key = toLowerString((*lineIt).substr(0, pos));
    if (_candidateFields.find(key) != _candidateFields.end()) {
      value =
          toLowerString((*lineIt).substr(pos + 2, (*lineIt).size() - pos - 2));
      // value 검증 필요
      (*dts.headerFields)[key] = value;
    }
    ++lineIt;
  }
  dts.linesBuffer->clear();
}

void RequestParser::checkContentLength(RequestDts &dts) {
  if (dts.body->empty() ||
      (*dts.headerFields)["transfer-encoding"] == "chunked")
    return;
  ssize_t contentLength =
      std::strtol((*dts.headerFields)["content-length"].c_str(), NULL, 10);
  if (dts.body->size() >= contentLength) {
    *dts.body = dts.body->substr(0, contentLength);
    *dts.isParsed = true;
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
    throw(_statusCode = NOT_FOUND);
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
      std::cout << "actual path: " << dts.path << '\n';
#endif
      if (this->checkPathForm(dts) == false) throw(_statusCode = NOT_FOUND);
      return;
    }
    if (currRoute == "/") defaultLocation = it;
    ++it;
  }
  this->setDefaultLocation(defaultLocation, dts);
}

void RequestParser::parseRequest(RequestDts &requestDts) {
  this->splitLinesByCRLF(requestDts);
  this->parseRequestLine(requestDts);
  this->parseHeaderFields(requestDts);
  this->checkContentLength(requestDts);
}

RequestParser &RequestParser::getInstance() {
  static RequestParser instance;
  return instance;
}
