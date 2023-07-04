#pragma once

#include <string>

class IRequest {
 public:
  virtual ~IRequest() {}
  virtual void parseRequest(short port) = 0;
  virtual void parseRequest(const std::string &request, short port) = 0;
};