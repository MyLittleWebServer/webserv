#include "Response.hpp"

#include "Status.hpp"

Response::Response()
    : _responseFlag(false),
      _assembleFlag(false),
      _statusCode(this->_statusCode = OK) {}

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

bool Response::getResponseFlag(void) const { return (this->_responseFlag); }

void Response::createErrorResponse(void) {
  resetResponse();

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

void Response::addResponse(std::string msg) { _response += msg; }
void Response::setResponseParsed() {
  this->_assembleFlag = true;
  this->_responseFlag = true;
}
bool Response::isParsed() { return _responseFlag; }