#include "Response.hpp"

#include "Status.hpp"

Response::Response()
    : _responseFlag(false),
      _assembleFlag(false),
      _statusCode(CREATED) {}

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

void Response::createErrorResponse(void) {
  resetResponse();
  // redirection response (300). need to replace the url with actual url
  if (statusInfo[this->_statusCode].body == NULL) {
    this->setHeaderField("Location", "http://example.com/redirect_url");
    this->_responseFlag = true;
    return;
  }
  // response for status code 400 ~ 500
  this->setHeaderField("Content-Type", "text/plain");
  this->addBody(statusInfo[this->_statusCode].body);
  this->addBody("\r\n");
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