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

void Response::create300Response(RequestDts &dts) {
  if (dts.path->empty()) *dts.path = "/";
  setHeaderField("Location", *dts.path);
  assembleResponse();
}

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

void Response::assembleResponseLine(void) {
  _response = "HTTP/1.1 ";
  _response += statusInfo[_statusCode].code;
  _response += " ";
  _response += statusInfo[_statusCode].message;
  _response += "\r\n";
  std::cout << _response << std::endl;
}

void Response::resetResponse(void) {
  if (_responseFlag == false) return;
  _response.clear();
  _responseFlag = false;
}

void Response::setResponseParsed() {
  _assembleFlag = true;
  _responseFlag = true;
}
bool Response::isParsed() { return _responseFlag; }

void Response::assembleResponse(void) {
  assembleResponseLine();
  putHeaderFields();
  putBody();
}

void Response::putHeaderFields(void) {
  std::map<std::string, std::string>::const_iterator it = _headerFields.begin();
  std::map<std::string, std::string>::const_iterator end = _headerFields.end();

  for (; it != end; ++it) {
    _response += it->first + ": " + it->second + "\r\n";
  }
}

void Response::putBody(void) {
  if (_body.empty() == true) {
    _response += "\r\n";
    return;
  }
  _response += "\r\n" + _body;
}

void Response::setStatusCode(Status code) { _statusCode = code; }

void Response::setHeaderField(const std::string &key,
                              const std::string &value) {
  _headerFields[key] = value;
}

void Response::eraseHeaderField(const std::string &key) {
  _headerFields.erase(key);
}

void Response::addBody(const std::string &str) { _body += str; }

void Response::setBody(const std::string &str) { _body = str; }

void Response::configureErrorPages(RequestDts &dts) {
  IServerConfig &serverConfig = **dts.matchedServer;

  std::string path =
      serverConfig.getErrorPage() + statusInfo[*dts.statusCode].code + ".html";

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

void Response::clear() {
  _response.clear();
  _responseFlag = false;
  _assembleFlag = false;
  _statusCode = E_200_OK;
  _body.clear();
  _headerFields.clear();
}
