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
#include "Status.hpp"
#include "Utils.hpp"

typedef struct RequestDts {
  std::string* request;
  std::string* method;
  std::string* path;
  std::string* protocol;
  std::string* CGI;
  std::list<std::string>* linesBuffer;
  std::map<std::string, std::string>* headerFields;
  std::map<std::string, std::string>* serverConf;
  IServerConfig* matchedServer;
  ILocationConfig* matchedLocation;
} RequestDts;

class IRequestParser {
 public:
  virtual ~IRequestParser(){};

  virtual void parseRequest(RequestDts& dts) = 0;
  virtual void matchServerConf(short port, RequestDts& dts) = 0;
  virtual void validatePath(RequestDts& dts) = 0;
};