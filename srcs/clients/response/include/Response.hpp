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
  std::string _body;
  void resetResponse();

 public:
  virtual void createErrorResponse(void);
  bool getResponseFlag(void) const;
  const std::string &getResponse(void) const;
  void assembleResponseLine(void);
};
