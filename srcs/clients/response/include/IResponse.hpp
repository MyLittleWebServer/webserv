#pragma once

class IResponse {
 public:
  virtual ~IResponse() {}
  virtual bool getResponseFlag(void) const = 0;
  virtual const std::string &getResponse(void) const = 0;
  virtual void assembleResponseLine(void) = 0;
};
