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
 private:
  Request();

 public:
  Request(std::string &request);
  virtual ~Request();
  Request(const Request &src);
  Request &operator=(const Request &src);

 private:
  std::string _request;
  std::string _method;
  std::string _path;
  std::string _protocol;
  std::string _CGI;
  Status _statusCode;
  IRequestParser &_parser;
  RequestDts _request_parser_dts;

  std::list<std::string> _linesBuffer;
  std::map<std::string, std::string> _headerFields;
  std::map<std::string, std::string> _serverConf;
  IServerConfig *_matchedServer;
  ILocationConfig *_matchedLocation;

  void initDts();

 public:
  void parseRequest(void);
  void matchServerConf(short port);
  void validatePath(void);
};
