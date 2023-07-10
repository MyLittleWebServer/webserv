#pragma once

#include <iostream>

#include "Config.hpp"
#include "IResponse.hpp"
#include "Status.hpp"
#include "Utils.hpp"

class Response : public IResponse {
 public:
  Response();
  virtual ~Response();
  Response(const Response &src);
  Response &operator=(const Response &src);

 private:
  std::string _response;
  bool _responseFlag;
  bool _assembleFlag;
  Status _statusCode;
  std::map<std::string, std::string> _headerFields;
  std::string _body;

  void resetResponse();

  void assembleResponseLine(void);
  void putHeaderFields(void);
  void putBody(void);

 public:
  virtual void createErrorResponse(Status statusCode);
  bool getResponseFlag(void) const;
  const std::string &getResponse(void) const;
  const std::string &getBody(void) const;
  std::string &getFieldValue(const std::string &key);

  void assembleResponse(void);

  void eraseHeaderField(const std::string &key);
  void addBody(const std::string &str);

  void setHeaderField(const std::string &key, const std::string &value);
  void setBody(const std::string &str);
  void setResponseParsed();
  bool isParsed();
};
