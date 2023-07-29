#pragma once

#include <string>

#include "Status.hpp"

class IResponse {
 public:
  virtual ~IResponse() {}
  virtual bool getResponseFlag(void) const = 0;
  virtual Status getStatus(void) = 0;
  virtual const std::string &getResponse(void) const = 0;
  virtual const std::string &getBody(void) const = 0;
  virtual std::string &getFieldValue(const std::string &key) = 0;
  virtual void assembleResponse(void) = 0;
  virtual void eraseHeaderField(const std::string &key) = 0;
  virtual void addBody(const std::string &str) = 0;
  virtual void setHeaderField(const std::string &key,
                              const std::string &value) = 0;
  virtual void setBody(const std::string &str) = 0;
  virtual void setStatusCode(Status code) = 0;
  virtual void setResponseParsed() = 0;
  virtual bool isParsed() = 0;

  virtual void setCookie(std::string &str) = 0;
  virtual void clear(void) = 0;
};
