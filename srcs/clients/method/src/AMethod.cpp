#include "AMethod.hpp"

#include "Status.hpp"
#include "Utils.hpp"
#include "errorMessage.hpp"

std::set<std::string> AMethod::_candidateFields;

void AMethod::initCandidateFields(void) {
  AMethod::_candidateFields.insert("host");
  AMethod::_candidateFields.insert("accept");
  AMethod::_candidateFields.insert("accept-language");
  AMethod::_candidateFields.insert("accept-encoding");
  AMethod::_candidateFields.insert("accept-charset");
  AMethod::_candidateFields.insert("authorization");
  AMethod::_candidateFields.insert("content-type");
  AMethod::_candidateFields.insert("connection");
  AMethod::_candidateFields.insert("user-agent");
  AMethod::_candidateFields.insert("content-length");
  AMethod::_candidateFields.insert("content-language");
  AMethod::_candidateFields.insert("content-encoding");
  AMethod::_candidateFields.insert("content-range");
  AMethod::_candidateFields.insert("content-length");
  AMethod::_candidateFields.insert("content-base");
  AMethod::_candidateFields.insert("content-location");
  AMethod::_candidateFields.insert("content-range");
  AMethod::_candidateFields.insert("keep-alive");
  AMethod::_candidateFields.insert("referer");
  AMethod::_candidateFields.insert("cookie");
  AMethod::_candidateFields.insert("last-modified");
  AMethod::_candidateFields.insert("if-modified-since");
  AMethod::_candidateFields.insert("date");
  AMethod::_candidateFields.insert("server");
  AMethod::_candidateFields.insert("www-authenticate");
  AMethod::_candidateFields.insert("retry-after");
  AMethod::_candidateFields.insert("location");
  AMethod::_candidateFields.insert("content-md5");
  AMethod::_candidateFields.insert("cache-control");
  AMethod::_candidateFields.insert("pragma");
  AMethod::_candidateFields.insert("expires");
  AMethod::_candidateFields.insert("age");
  AMethod::_candidateFields.insert("allow");
  AMethod::_candidateFields.insert("etag");
  AMethod::_candidateFields.insert("accept-ranges");
  AMethod::_candidateFields.insert("set-cookie");
  AMethod::_candidateFields.insert("vary");
  AMethod::_candidateFields.insert("x-frame-options");
  AMethod::_candidateFields.insert("x-xss-protection");
  AMethod::_candidateFields.insert("x-content-type-options");
  AMethod::_candidateFields.insert("x-forwarded-server");
  AMethod::_candidateFields.insert("x-forwarded-proto");
  AMethod::_candidateFields.insert("x-real-ip");
  AMethod::_candidateFields.insert("x-request-id");
  AMethod::_candidateFields.insert("x-correlation-id");
  AMethod::_candidateFields.insert("x-csrf-token");
  AMethod::_candidateFields.insert("x-device-user-agent");
}

AMethod::AMethod() {}
AMethod::AMethod(Status statusCode) : _statusCode(statusCode) {}
AMethod::AMethod(std::string &request)
    : _request(request), _responseFlag(false) {}
AMethod::~AMethod() {}

void AMethod::splitLinesByCRLF(void) {
  size_t pos = 0;
  size_t delimeter = this->_request.find("\r\n");
  while (delimeter != std::string::npos) {
    std::string chunk = this->_request.substr(pos, delimeter);
    this->_linesBuffer.push_back(chunk);
    pos = delimeter + 2;
    delimeter = this->_request.find("\r\n", pos);
  }
}

void AMethod::parseRequestLine(void) {
  const std::string &firstLine(this->_linesBuffer.front());
  int delim_cnt = 0;
  size_t pos = firstLine.find(" ", 0);
  while (pos != std::string::npos) {
    delim_cnt++;
    pos = firstLine.find(" ", ++pos);
  }
  if (delim_cnt != 2) throw(this->_statusCode = BAD_REQUEST);

  std::istringstream iss(firstLine);
  this->_linesBuffer.pop_front();
  iss >> this->_method >> this->_path >> this->_protocol;

  std::cout << "method: " << this->_method << std::endl;
  std::cout << "path: " << this->_path << std::endl;
  std::cout << "protocol: " << this->_protocol << std::endl;
  if (this->_method == "" || this->_path == "" || this->_protocol == "")
    throw(this->_statusCode = BAD_REQUEST);
}

void AMethod::parseHeaderFields(void) {
  std::list<std::string>::const_iterator lineIt = this->_linesBuffer.begin();
  std::list<std::string>::const_iterator lineEnd = this->_linesBuffer.end();

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
      this->_headerFields[key] = value;
    }
    ++lineIt;
    this->_linesBuffer.pop_front();
    if (checkBodyExistance(lineIt)) {
      this->_linesBuffer.pop_front();
      return;
    }
  }
}

bool AMethod::checkBodyExistance(
    std::list<std::string>::const_iterator it) const {
  return (!this->_linesBuffer.empty() && *it == "");
}

bool AMethod::checkPathForm() {
  std::string::const_iterator iter = this->_path.begin();
  std::string::const_iterator end = this->_path.end();
  if (*iter == '/') ++iter;
  while (iter != end) {
    if (*iter == '/' && *(iter + 1) == '/') return false;
    ++iter;
  }
  return true;
}

void AMethod::setDefaultLocation(
    std::list<ILocationConfig *>::const_iterator defaultLocation) {
  this->_matchedLocation = *defaultLocation;
  this->_path.erase(0, 1);  // remove the first '/'
  this->_path = (*defaultLocation)->getRoot() + this->_path;
  this->_path.erase(0, 1);
}

const std::string &AMethod::getResponse(void) const {
  return (this->_response);
}

bool AMethod::getResponseFlag(void) const { return (this->_responseFlag); }

void AMethod::matchServerConf(short port) {
  this->_matchedServer = NULL;
  Config &config = Config::getInstance();
  std::list<IServerConfig *> serverInfo = config.getServerConfigs();
  std::list<IServerConfig *>::iterator it = serverInfo.begin();
  if (serverInfo.empty()) throw(42.42);
  while (it != serverInfo.end()) {
    if ((*it)->getListen() != port) {
      ++it;
      continue;
    }
    if ((*it)->getServerName() == this->_headerFields["host"]) {
      this->_matchedServer = *it;
      return;
    } else
      this->_matchedServer = *it;
    ++it;
  }
  if (this->_matchedServer == NULL) {
#ifdef DEBUG_MSG
    std::cout << "no matched server" << std::endl;
#endif
    throw(this->_statusCode = NOT_FOUND);
  }
}

std::string AMethod::getFirstTokenOfPath(void) const {
  size_t pos = this->_path.find("/", 1);
  size_t end;
  if (pos == std::string::npos)
    end = this->_path.size();
  else
    end = pos;
  return (this->_path.substr(0, end));
}

// /root//dir/test.txt
// GET /dir/test.txt/ hTML/1.1
void AMethod::validatePath(void) {
  std::string firstToken = getFirstTokenOfPath();
#ifdef DEBUG_MSG
  std::cout << "firstToken: " << firstToken << std::endl;
#endif
  std::list<ILocationConfig *>::const_iterator it =
      this->_matchedServer->getLocationConfigs().begin();
  std::list<ILocationConfig *>::const_iterator endIt =
      this->_matchedServer->getLocationConfigs().end();
  std::list<ILocationConfig *>::const_iterator defaultLocation;
  while (it != endIt) {
    const std::string &currRoute = (*it)->getRoute();
#ifdef DEBUG_MSG
    std::cout << "currRoute: " << currRoute << std::endl;
#endif
    if (currRoute == firstToken) {
      this->_matchedLocation = *it;
      this->_path.erase(0, firstToken.size());
      if (this->_path.size() != 0 && this->_path[0] == '/')
        this->_path.erase(0, 1);  // remove this because there is already a
                                  // slash at the end of root path
      this->_path = (*it)->getRoot() + this->_path;
      this->_path.erase(0, 1);  // remove the first '/'
#ifdef DEBUG_MSG
      std::cout << "actual path: " << this->_path << '\n';
#endif
      if (this->checkPathForm() == false) throw(this->_statusCode = NOT_FOUND);
      return;
    }
    if (currRoute == "/") defaultLocation = it;
    ++it;
  }
  this->setDefaultLocation(defaultLocation);
}

void AMethod::parseRequest(void) {
  this->splitLinesByCRLF();
  this->parseRequestLine();
  this->parseHeaderFields();
}

void AMethod::assembleResponseLine(void) {
  this->_response = "HTTP/1.1 ";
  this->_response += statusInfo[this->_statusCode].code;
  this->_response += " ";
  this->_response += statusInfo[this->_statusCode].message;
  this->_response += "\r\n";
}

void AMethod::createErrorResponse(void) {
  this->assembleResponseLine();
  // redirection response (300). need to replace the url with actual url
  if (statusInfo[this->_statusCode].body == NULL) {
    this->_response += "Location: ";
    this->_response += "http://example.com/redirect_url\r\n";
    this->_responseFlag = true;
    return;
  }
  // response for status code 400 ~ 500
  this->_response += "Content-Type: text/plain\r\n";
  this->_response += "Content-Length: ";
  this->_response += itos(statusInfo[this->_statusCode].contentLength);
  this->_response += "\r\n\r\n";
  this->_response += statusInfo[this->_statusCode].body;
  this->_response += "\r\n";
  this->_responseFlag = true;
}
