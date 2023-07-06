#pragma once

#include <string>

class IResponse {
 public:
  virtual ~IResponse() {}
  virtual bool getResponseFlag(void) const = 0;
  virtual const std::string &getResponse(void) const = 0;
  virtual void assembleResponse(void) = 0;
  virtual void setResponseParsed() = 0;
  virtual bool isParsed() = 0;
};
