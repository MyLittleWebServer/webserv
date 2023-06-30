#include "Request.hpp"

Request::Request() : _parser(RequestParser::getInstance()) { initDts(); }

Request::Request(std::string &request)
    : _parser(RequestParser::getInstance()), _request(request) {
  initDts();
}

Request::~Request() {}

Request::Request(const Request &src)
    : _parser(RequestParser::getInstance()), _request(src._request) {
  *this = src;
}

Request &Request::operator=(const Request &src) {
  if (this != &src) {
    this->_path = src._path;
    this->_protocol = src._protocol;
    this->_statusCode = src._statusCode;
    this->_linesBuffer = src._linesBuffer;
    this->_headerFields = src._headerFields;
    this->_serverConf = src._serverConf;
    this->_matchedServer = src._matchedServer;
    this->_matchedLocation = src._matchedLocation;
    this->_CGI = src._CGI;
    this->_method = src._method;
    this->_request = src._request;
    this->_statusCode = src._statusCode;
  }
  initDts();
  return *this;
}

void Request::initDts() {
  _request_parser_dts.request = &_request;
  _request_parser_dts.method = &_method;
  _request_parser_dts.path = &_path;
  _request_parser_dts.protocol = &_protocol;
  _request_parser_dts.CGI = &_CGI;
  _request_parser_dts.linesBuffer = &_linesBuffer;
  _request_parser_dts.headerFields = &_headerFields;
  _request_parser_dts.serverConf = &_serverConf;
  _request_parser_dts.matchedServer = _matchedServer;
  _request_parser_dts.matchedLocation = _matchedLocation;
}

void Request::parseRequest(void) { _parser.parseRequest(_request_parser_dts); }

void Request::matchServerConf(short port) {
  _parser.matchServerConf(port, _request_parser_dts);
}

void Request::validatePath(void) { _parser.validatePath(_request_parser_dts); }