#pragma once

#include <iostream>
#include <list>
#include <map>
#include <set>
#include <sstream>
#include <string>

#include "Config.hpp"
#include "Status.hpp"

typedef struct RequestDts {
  Status* statusCode;
  std::string* request;
  std::string* method;
  std::string* path;
  std::string* originalPath;
  std::string* anchor;
  std::string* protocol;
  std::string* cgi_path;
  std::string* body;
  std::list<std::string>* linesBuffer;
  std::map<std::string, std::string>* headerFields;
  std::map<std::string, std::string>* queryStringElements;
  std::string* query_string;
  std::map<std::string, std::string>* serverConf;
  IServerConfig** matchedServer;
  ILocationConfig** matchedLocation;
  bool* isParsed;
  bool* is_cgi;
  unsigned long* contentLength;
} RequestDts;

class IRequestParser {
 public:
  virtual ~IRequestParser(){};

  virtual void parseRequest(RequestDts& dts, short port) = 0;
};
