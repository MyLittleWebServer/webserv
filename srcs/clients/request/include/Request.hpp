#pragma once

#include "Config.hpp"
#include "IRequest.hpp"
#include "RequestParser.hpp"

class Request : public IRequest {
 public:
  Request();
  Request(const std::string &request);
  virtual ~Request();
  Request(const Request &src);
  Request &operator=(const Request &src);

 private:
  IRequestParser &_parser;
  RequestDts _request_parser_dts;

 private:
  bool _isParsed;
  bool _is_cgi;
  bool _is_expect_100;
  bool _is_session;

  size_t _contentLength;

  std::string _request;
  std::string _method;
  std::string _path;
  std::string _originalPath;
  std::string _anchor;
  std::string _protocol;
  std::string _cgi_path;
  std::string _body;
  std::string _query_string;

  Status _statusCode;

  std::list<std::string> _linesBuffer;
  std::map<std::string, std::string> _headerFields;
  std::map<std::string, std::string> _cookieMap;
  std::map<std::string, std::string> _queryStringElements;
  std::map<std::string, std::string> _serverConf;

  IServerConfig *_matchedServer;
  ILocationConfig *_matchedLocation;

 private:
  void initDts();
  void initMember();

 public:
  void parseRequest(short port);
  void parseRequest(const std::string &request, short port);
  void setStatusCode(Status code);

  RequestDts &getRequestParserDts(void);
  const std::string &getRequest(void) const;
  const std::string &getMethod(void) const;
  const std::string &getPath(void) const;
  const std::string &getProtocol(void) const;
  const std::string &getCgiPath(void) const;
  const std::string &getBody(void) const;
  const std::string &getQueryString(void) const;
  const std::string &getHeaderField(const std::string &key) const;
  void setHeaderField(const std::string &key, const std::string &value);

  size_t getContentLength(void) const;
  const std::map<std::string, std::string> &getQueryStringElements(void) const;
  Status getStatusCode(void) const;
  std::map<std::string, std::string> &getHeaderFields(void);
  IServerConfig *getMatchedServer(void) const;

  const bool &isParsed(void) const;
  const bool &isCgi(void) const;
  const bool &isExpect100(void) const;
  const bool &isSession(void) const;

  void clear(void);
};

std::ostream &operator<<(std::ostream &os, const Request &request);
