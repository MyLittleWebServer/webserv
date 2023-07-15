#include "Response.hpp"

#include "Status.hpp"

Response::Response()
    : _responseFlag(false), _assembleFlag(false), _statusCode(E_200_OK) {}

Response::~Response() {}

Response::Response(const Response &src) { *this = src; }

Response &Response::operator=(const Response &src) {
  if (this != &src) {
    this->_response = src._response;
    this->_responseFlag = src._responseFlag;
    this->_assembleFlag = src._assembleFlag;
    this->_statusCode = src._statusCode;
    this->_body = src._body;
  }
  return (*this);
}

const std::string &Response::getResponse(void) const {
  return (this->_response);
}

const std::string &Response::getBody(void) const { return (this->_body); }

bool Response::getResponseFlag(void) const { return (this->_responseFlag); }

Status Response::getStatus(void) { return (this->_statusCode); }

std::string &Response::getFieldValue(const std::string &key) {
  return (this->_headerFields[key]);
}

void Response::createErrorResponse(RequestDts &dts) {
  resetResponse();
  this->_statusCode = *dts.statusCode;
  // redirection response (300). need to replace the url with actual url
  // if (dts.statusCode != NULL && statusInfo[*dts.statusCode].body == NULL) {
  //   this->setHeaderField("Location", "http://example.com/redirect_url");
  //   this->_responseFlag = true;
  //   return;
  // }
  // response for status code 400 ~ 500
  if (*dts.matchedServer != NULL) {
    this->configureErrorPages(dts);
  } else {
    this->addBody(statusInfo[*dts.statusCode].body);
    this->addBody("\r\n");
  }
  this->setHeaderField("Content-Length", itos(this->_body.size()));
  this->assembleResponse();

  this->_responseFlag = true;
}

void Response::assembleResponseLine(void) {
  this->_response = "HTTP/1.1 ";
  this->_response += statusInfo[this->_statusCode].code;
  this->_response += " ";
  this->_response += statusInfo[this->_statusCode].message;
  this->_response += "\r\n";
  std::cout << _response << std::endl;
  std::cout << _response << std::endl;
}

void Response::resetResponse(void) {
  if (this->_responseFlag == false) return;
  this->_response.clear();
  this->_responseFlag = false;
}

void Response::setResponseParsed() {
  this->_assembleFlag = true;
  this->_responseFlag = true;
}
bool Response::isParsed() { return _responseFlag; }

void Response::assembleResponse(void) {
  this->assembleResponseLine();
  this->putHeaderFields();
  this->putBody();
}

void Response::putHeaderFields(void) {
  std::map<std::string, std::string>::const_iterator it =
      this->_headerFields.begin();
  std::map<std::string, std::string>::const_iterator end =
      this->_headerFields.end();

  for (; it != end; ++it) {
    this->_response += it->first + ": " + it->second + "\r\n";
  }
}

void Response::putBody(void) {
  if (this->_body.empty() == true) return;
  this->_response += "\r\n" + this->_body;
}

void Response::setStatusCode(Status code) { this->_statusCode = code; }

void Response::setHeaderField(const std::string &key,
                              const std::string &value) {
  this->_headerFields[key] = value;
}

void Response::eraseHeaderField(const std::string &key) {
  this->_headerFields.erase(key);
}

void Response::addBody(const std::string &str) { this->_body += str; }

void Response::setBody(const std::string &str) { this->_body = str; }

void Response::setResponse(std::string response) { this->_response = response; }

void Response::configureErrorPages(RequestDts &dts) {
  IServerConfig &serverConfig = **dts.matchedServer;

  std::string path =
      serverConfig.getErrorPage() + statusInfo[*dts.statusCode].code + ".html";

  if (path[0] == '/') {
    path = path.substr(1);
  }
  std::ifstream file(path.c_str(), std::ios::in);
  if (file.is_open() == false) {
    this->setHeaderField("Content-Type", "text/plain");
    this->addBody(statusInfo[*dts.statusCode].body);
    this->addBody("\r\n");
    return;
  } else {
    this->setHeaderField("Content-Type", "text/html; charset=UTF-8");
    std::string buff;
    while (std::getline(file, buff)) {
      this->addBody(buff);
      this->addBody("\r\n");
    }
    file.close();
  }
}

void Response::clear() {
  this->_response.clear();
  this->_responseFlag = false;
  this->_assembleFlag = false;
  this->_statusCode = E_200_OK;
  this->_body.clear();
  this->_headerFields.clear();
}
