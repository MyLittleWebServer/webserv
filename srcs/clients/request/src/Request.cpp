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
    _path = src._path;
    _protocol = src._protocol;
    _statusCode = src._statusCode;
    _linesBuffer = src._linesBuffer;
    _headerFields = src._headerFields;
    _cookieMap = src._cookieMap;
    _serverConf = src._serverConf;
    _matchedServer = src._matchedServer;
    _matchedLocation = src._matchedLocation;
    _cgi_path = src._cgi_path;
    _method = src._method;
    _request = src._request;
    _statusCode = src._statusCode;
    _query_string = src._query_string;
    _queryStringElements = src._queryStringElements;
    _body = src._body;
    _anchor = src._anchor;
    _isParsed = src._isParsed;
    _contentLength = src._contentLength;
    _is_cgi = src._is_cgi;
    _is_session = src._is_session;
    _originalPath = src._originalPath;
    _is_expect_100 = src._is_expect_100;
    _valid_flag = src._valid_flag;
  }
  initDts();
  return *this;
}

void Request::initMember() {
  _isParsed = false;
  _is_cgi = false;
  _is_expect_100 = false;
  _is_session = false;
  _valid_flag = false;
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
  _headerFields["dummy"] = "";
  _cookieMap.clear();
  _queryStringElements.clear();
  _serverConf.clear();

  _matchedServer = NULL;
  _matchedLocation = NULL;
}

void Request::initDts() {
  _request_parser_dts.isParsed = &_isParsed;
  _request_parser_dts.is_cgi = &_is_cgi;
  _request_parser_dts.is_session = &_is_session;
  _request_parser_dts.valid_flag = &_valid_flag;

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
  _request_parser_dts.cookieMap = &_cookieMap;
  _request_parser_dts.queryStringElements = &_queryStringElements;
  _request_parser_dts.serverConf = &_serverConf;

  _request_parser_dts.matchedServer = &_matchedServer;
  _request_parser_dts.matchedLocation = &_matchedLocation;
  _request_parser_dts.is_expect_100 = &_is_expect_100;
}

void Request::parseRequest(short port) {
  initMember();
  _parser.parseRequest(_request_parser_dts, port);
}

void Request::parseRequest(const std::string &request, short port) {
  initMember();
  _request += request;
  _parser.parseRequest(_request_parser_dts, port);
}

RequestDts &Request::getRequestParserDts(void) { return _request_parser_dts; }

const std::string &Request::getRequest(void) const { return _request; }
const std::string &Request::getMethod(void) const { return _method; }
const std::string &Request::getPath(void) const { return _path; }
const std::string &Request::getProtocol(void) const { return _protocol; }
const std::string &Request::getCgiPath(void) const { return _cgi_path; }
const std::string &Request::getBody(void) const { return _body; }
const std::string &Request::getQueryString(void) const { return _query_string; }

size_t Request::getContentLength(void) const { return _contentLength; }

Status Request::getStatusCode(void) const { return _statusCode; }
std::map<std::string, std::string> &Request::getHeaderFields(void) {
  return _headerFields;
}
const std::string &Request::getHeaderField(const std::string &key) const {
  std::map<std::string, std::string>::const_iterator iter =
      _headerFields.find(toLowerString(key));
  if (iter != _headerFields.end()) {
    return iter->second;
  }
  return _headerFields.at("dummy");
}
const std::map<std::string, std::string> &Request::getQueryStringElements(
    void) const {
  return _queryStringElements;
}

IServerConfig *Request::getMatchedServer(void) const {
  return *_request_parser_dts.matchedServer;
}

const bool &Request::isParsed() const { return _isParsed; }

const bool &Request::isCgi() const { return _is_cgi; }

const bool &Request::isSession() const { return _is_session; }

std::ostream &operator<<(std::ostream &os, const Request &request) {
  os << request.getMethod() << request.getRequest() << request.getPath()
     << request.getProtocol() << request.getCgiPath() << request.getBody()
     << std::endl;
  return os;
}

void Request::clear() { initMember(); }

void Request::setHeaderField(const std::string &key, const std::string &value) {
  _headerFields[toLowerString(key)] = value;
}

const bool &Request::isExpect100() const { return _is_expect_100; }

void Request::setStatusCode(Status code) { _statusCode = code; }
