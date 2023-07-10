#pragma once

#include <string>

#include "RequestParser.hpp"

class IRequest {
 public:
  virtual ~IRequest() {}
  virtual void parseRequest(short port) = 0;
  virtual void parseRequest(const std::string &request, short port) = 0;

  virtual RequestDts &getRequestParserDts(void) = 0;
  virtual const std::string &getRequest(void) const = 0;
  virtual const std::string &getMethod(void) const = 0;
  virtual const std::string &getPath(void) const = 0;
  virtual const std::string &getProtocol(void) const = 0;
  virtual const std::string &getCgiPath(void) const = 0;
  virtual const std::string &getBody(void) const = 0;
  virtual size_t getContentLength(void) const = 0;
  virtual const std::string &getQueryString(void) const = 0;
  virtual const std::map<std::string, std::string> &getQueryStringElements(
      void) const = 0;
  virtual Status getStatusCode(void) const = 0;
  virtual std::map<std::string, std::string> &getHeaderFields(void) = 0;
  virtual IServerConfig *getMatchedServer(void) const = 0;

  virtual const std::string getHeaderField(std::string key) const = 0;
};
