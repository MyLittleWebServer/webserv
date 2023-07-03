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
  Status* statusCode;
  std::string* request;
  std::string* method;
  std::string* path;
  std::string* anchor;
  std::string* protocol;
  std::string* CGI;
  std::string* body;
  std::list<std::string>* linesBuffer;
  std::map<std::string, std::string>* headerFields;
  std::map<std::string, std::string>* queryString;
  std::map<std::string, std::string>* serverConf;
  IServerConfig* matchedServer;
  ILocationConfig* matchedLocation;
  bool* isParsed;
  unsigned long* contentLength;
} RequestDts;

class IRequestParser {
 public:
  virtual ~IRequestParser(){};

  virtual void parseRequest(RequestDts& dts, short port) = 0;
};
