#pragma once

#include <string>

class IResponse {
 public:
  virtual ~IResponse() {}
  virtual bool getResponseFlag(void) const = 0;
  virtual const std::string &getResponse(void) const = 0;
  virtual void assembleResponseLine(void) = 0;
  virtual void addResponse(std::string msg) = 0;
  virtual void setResponseParsed() = 0;
  virtual bool isParsed() = 0;
};
