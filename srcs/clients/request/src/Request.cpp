#include "Request.hpp"

Request::Request() : _parser(RequestParser::getInstance()) {
  initMember();
  initDts();
}

Request::Request(const std::string &request)
    : _parser(RequestParser::getInstance()), _request(request) {
  initMember();
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
    this->_cgi_path = src._cgi_path;
    this->_method = src._method;
    this->_request = src._request;
    this->_statusCode = src._statusCode;
    this->_query_string = src._query_string;
    this->_queryStringElements = src._queryStringElements;
    this->_body = src._body;
    this->_anchor = src._anchor;
    this->_isParsed = src._isParsed;
    this->_contentLength = src._contentLength;
    this->_is_cgi = src._is_cgi;
  }
  initDts();
  return *this;
}

void Request::initMember() {
  _isParsed = false;
  _is_cgi = false;

  _contentLength = 0;

  _request.clear();
  _method.clear();
  _path.clear();
  _originalPath.clear();
  _anchor.clear();
  _protocol.clear();
  _cgi_path.clear();
  _body.clear();
  _query_string.clear();

  _headerFields.clear();
  _queryStringElements.clear();
  _serverConf.clear();

  _matchedServer = NULL;
  _matchedLocation = NULL;
}

void Request::initDts() {
  _request_parser_dts.isParsed = &_isParsed;
  _request_parser_dts.is_cgi = &_is_cgi;

  _request_parser_dts.contentLength = &_contentLength;

  _request_parser_dts.request = &_request;
  _request_parser_dts.method = &_method;
  _request_parser_dts.path = &_path;
  _request_parser_dts.originalPath = &_originalPath;
  _request_parser_dts.anchor = &_anchor;
  _request_parser_dts.protocol = &_protocol;
  _request_parser_dts.cgi_path = &_cgi_path;
  _request_parser_dts.body = &_body;
  _request_parser_dts.query_string = &_query_string;

  _request_parser_dts.statusCode = &_statusCode;

  _request_parser_dts.linesBuffer = &_linesBuffer;
  _request_parser_dts.headerFields = &_headerFields;
  _request_parser_dts.queryStringElements = &_queryStringElements;
  _request_parser_dts.serverConf = &_serverConf;

  _request_parser_dts.matchedServer = &_matchedServer;
  _request_parser_dts.matchedLocation = &_matchedLocation;
}

void Request::parseRequest(short port) {
  initMember();
  initDts();
  _parser.parseRequest(_request_parser_dts, port);
}

void Request::parseRequest(const std::string &request, short port) {
  initMember();
  _request += request;
  initDts();
  _parser.parseRequest(_request_parser_dts, port);
}

RequestDts &Request::getRequestParserDts(void) {
  return this->_request_parser_dts;
}

const std::string &Request::getRequest(void) const { return (this->_request); }
const std::string &Request::getMethod(void) const { return (this->_method); }
const std::string &Request::getPath(void) const { return (this->_path); }
const std::string &Request::getProtocol(void) const {
  return (this->_protocol);
}
const std::string &Request::getCgiPath(void) const { return (this->_cgi_path); }
const std::string &Request::getBody(void) const { return (this->_body); }
const std::string &Request::getQueryString(void) const { return _query_string; }

size_t Request::getContentLength(void) const { return (this->_contentLength); }

Status Request::getStatusCode(void) const { return (this->_statusCode); }
std::map<std::string, std::string> &Request::getHeaderFields(void) {
  return (this->_headerFields);
}
const std::string Request::getHeaderField(std::string key) const {
  key = toLowerString(key).substr(0, key.length());
  std::map<std::string, std::string>::const_iterator iter =
      _headerFields.find(key);
  if (iter != _headerFields.end()) {
    return iter->second;
  } else {
    return "";
  }
}
const std::map<std::string, std::string> &Request::getQueryStringElements(
    void) const {
  return (this->_queryStringElements);
}

IServerConfig *Request::getMatchedServer(void) const {
  return (*this->_request_parser_dts.matchedServer);
}

const bool &Request::isParsed() const { return _isParsed; }

const bool &Request::isCgi() const { return _is_cgi; }

std::ostream &operator<<(std::ostream &os, const Request &request) {
  os << request.getMethod() << request.getRequest() << request.getPath()
     << request.getProtocol() << request.getCgiPath() << request.getBody()
     << std::endl;
  return os;
}

void Request::clear() {
  initMember();
  initDts();
}

void Request::setHeaderField(std::string key, std::string value) {
  key = toLowerString(key).substr(0, key.length());
  _headerFields[key] = value;
}