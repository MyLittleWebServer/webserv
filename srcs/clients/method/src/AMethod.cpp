#include "AMethod.hpp"

#include "errorMessage.hpp"

AMethod::AMethod() {}
AMethod::AMethod(std::string &request)
    : _request(request), _responseFlag(false) {
  /* major-field */
  this->_headerFields["host"] = "";
  this->_headerFields["accept"] = "";
  this->_headerFields["accept-language"] = "";
  this->_headerFields["accept-encoding"] = "";
  this->_headerFields["accept-charset"] = "";
  this->_headerFields["authorization"] = "";
  this->_headerFields["content-type"] = "";
  this->_headerFields["connection"] = "";
  this->_headerFields["user-agent"] = "";
  /* sub-field */
  this->_headerFields["content-length"] = "";
  this->_headerFields["content-language"] = "";
  this->_headerFields["content-encoding"] = "";
  this->_headerFields["content-range"] = "";
  this->_headerFields["content-length"] = "";
  this->_headerFields["content-base"] = "";
  this->_headerFields["content-location"] = "";
  this->_headerFields["content-range"] = "";
  this->_headerFields["keep-alive"] = "";
  this->_headerFields["referer"] = "";
  this->_headerFields["cookie"] = "";
  this->_headerFields["last-modified"] = "";
  this->_headerFields["if-modified-since"] = "";
  this->_headerFields["date"] = "";
  this->_headerFields["server"] = "";
  this->_headerFields["www-authenticate"] = "";
  this->_headerFields["retry-after"] = "";
  this->_headerFields["location"] = "";
  this->_headerFields["content-md5"] = "";
  this->_headerFields["cache-control"] = "";
  this->_headerFields["pragma"] = "";
  this->_headerFields["expires"] = "";
  this->_headerFields["age"] = "";
  this->_headerFields["allow"] = "";
  this->_headerFields["etag"] = "";
  this->_headerFields["accept-ranges"] = "";
  this->_headerFields["set-cookie"] = "";
  this->_headerFields["vary"] = "";
  this->_headerFields["x-frame-options"] = "";
  this->_headerFields["x-xss-protection"] = "";
  this->_headerFields["x-content-type-options"] = "";
  this->_headerFields["x-forwarded-for"] = "";
  this->_headerFields["x-forwarded-host"] = "";
  this->_headerFields["x-forwarded-server"] = "";
  this->_headerFields["x-forwarded-proto"] = "";
  this->_headerFields["x-real-ip"] = "";
  this->_headerFields["x-request-id"] = "";
  this->_headerFields["x-correlation-id"] = "";
  this->_headerFields["x-csrf-token"] = "";
  this->_headerFields["x-device-user-agent"] = "";
}
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
  int delim_cnt = 0;
  size_t pos = 0;
  const std::string &firstLine(this->_linesBuffer.front());
  size_t delimeter = firstLine.find(" ");
  if (delimeter == std::string::npos) {
    throw std::runtime_error("request has bad indentation!");
  }
  while (delimeter != std::string::npos) {
    delim_cnt += 1;
    pos = delimeter + 1;
    delimeter = firstLine.find(" ", pos);
  }
  if (delim_cnt != 2) throw std::runtime_error("request has bad indentation!");
  std::istringstream iss(firstLine);
  this->_linesBuffer.pop_front();
  iss >> this->_method >> this->_path >> this->_protocol;

  // std::cout << "method: " << this->_method << std::endl;
  // std::cout << "path: " << this->_path << std::endl;
  // std::cout << "protocol: " << this->_protocol << std::endl;
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
    key = (*lineIt).substr(0, pos);
    value = (*lineIt).substr(pos + 2, (*lineIt).size() - pos - 2);
    // value 검증 필요
    this->_headerFields[key] = value;
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
  this->_path = (*defaultLocation)->getRoot() +
                this->_path.substr(1, this->_path.size() - 1);
}

const std::string &AMethod::getResponse(void) const {
  return (this->_response);
}

bool AMethod::getResponseFlag(void) const { return (this->_responseFlag); }

void AMethod::matchServerConf(short port) {
  this->_matchedServer = NULL;
  Config &config = Config::getInstance();
  std::list<IServerConfig *> &serverInfo = config.getServerConfigs();
  std::list<IServerConfig *>::iterator it = serverInfo.begin();
  while (it != serverInfo.end()) {
    if ((*it)->getListen() != port) {
      ++it;
      continue;
    }
    if ((*it)->getServerName() == this->_headerFields["host"]) {
      this->_matchedServer = *it;
      return;
    } else if (this->_matchedServer == NULL)
      this->_matchedServer = *it;
    ++it;
  }
}

// /root//dir/test.txt
// GET /dir/test.txt/ hTML/1.1
void AMethod::validatePath(void) {
  size_t pos = this->_path.find("/", 1);
  size_t end;
  if (pos == std::string::npos)
    end = 1;
  else
    end = pos;
  std::string firstToken = this->_path.substr(0, end);
  std::list<ILocationConfig *>::const_iterator it =
      this->_matchedServer->getLocationConfigs().begin();
  std::list<ILocationConfig *>::const_iterator endIt =
      this->_matchedServer->getLocationConfigs().end();
  std::list<ILocationConfig *>::const_iterator defaultLocation;
  while (it != endIt) {
    std::string &currRoute = (*it)->getRoute();
    if (currRoute == firstToken) {
      this->_matchedLocation = *it;
      if (end == pos) end++;
      this->_path =
          currRoute + this->_path.substr(end, this->_path.size() - end);
      if (this->checkPathForm() == false) this->_statusCode = NOT_FOUND;
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
  this->_response += statusCodes[this->_statusCode].code;
  this->_response += " ";
  this->_response += statusCodes[this->_statusCode].message;
  this->_response += "\r\n";
}

void AMethod::assembleResponseBody(void) {
  this->_response += "\r\n";
  if (statusCodes[this->_statusCode].body != NULL) {
    this->_response += statusCodes[this->_statusCode].body;
    return;
  }
}

void AMethod::createErrorResponse(void) {
  this->assembleResponseLine();
  // redirection response (300). need to replace the url with actual url
  if (statusCodes[this->_statusCode].body == NULL) {
    this->_response += "Location: ";
    this->_response += "http://example.com/redirect_url\r\n";
    this->_responseFlag = true;
    return;
  }
  // response for status code 400 ~ 500
  this->_response += "Content-Type: text/html\r\n";
  this->_response += "Content-Length: ";
  this->_response += itos(statusCodes[this->_statusCode].contentLength);
  this->_response += "\r\n\r\n";
  this->_response += statusCodes[this->_statusCode].body;
  this->_response += "\r\n";
  this->_responseFlag = true;
}
