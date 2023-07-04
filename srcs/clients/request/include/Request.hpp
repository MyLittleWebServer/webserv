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
  bool _isParsed;
  bool _is_cgi;
  size_t _contentLength;

  std::string _request;
  std::string _method;
  std::string _path;
  std::string _anchor;
  std::string _protocol;
  std::string _cgi_path;
  std::string _body;
  Status _statusCode;
  IRequestParser &_parser;
  RequestDts _request_parser_dts;

  std::list<std::string> _linesBuffer;
  std::map<std::string, std::string> _headerFields;
  std::map<std::string, std::string> _queryString;
  std::map<std::string, std::string> _serverConf;
  IServerConfig *_matchedServer;
  ILocationConfig *_matchedLocation;

  void initDts();
  void initMember();

 public:
  void parseRequest(short port);
  void parseRequest(const std::string &request, short port);

  void appendRequest(const std::string &str);
  void appendRequest(const char *str);

  RequestDts &getRequestParserDts(void);
  const std::string &getRequest(void) const;
  const std::string &getMethod(void) const;
  const std::string &getPath(void) const;
  const std::string &getProtocol(void) const;
  const std::string &getCgiPath(void) const;
  const std::string &getBody(void) const;
  const std::map<std::string, std::string> &getQueryString(void) const;
  Status getStatusCode(void) const;
  std::map<std::string, std::string> &getHeaderFields(void);
  const bool &isParsed(void) const;
  const bool &isCgi(void) const;
};
