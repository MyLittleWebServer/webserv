#pragma once

#include <iostream>
#include <list>
#include <map>
#include <set>
#include <sstream>
#include <string>

#include "CandidateFields.hpp"
#include "Config.hpp"
#include "IRequest.hpp"
#include "RequestParser.hpp"
#include "Status.hpp"

class Request : public IRequest {
 public:
  Request();
  Request(std::string &request);
  virtual ~Request();
  Request(const Request &src);
  Request &operator=(const Request &src);

 private:
  bool _isParsed;

  std::string _request;
  std::string _method;
  std::string _path;
  std::string _anchor;
  std::string _protocol;
  std::string _CGI;
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
  void parseRequest(void);
  void matchServerConf(short port);
  void validatePath(void);

  void appendRequest(const std::string &str);
  void appendRequest(const char *str);

  const std::string &getRequest(void) const;
  const std::string &getMethod(void) const;
  const std::string &getPath(void) const;
  const std::string &getProtocol(void) const;
  const std::string &getCGI(void) const;
  const std::string &getBody(void) const;
  const std::map<std::string, std::string> &getQueryString(void) const;
  Status getStatusCode(void) const;
  std::map<std::string, std::string> &getHeaderFields(void);
};
