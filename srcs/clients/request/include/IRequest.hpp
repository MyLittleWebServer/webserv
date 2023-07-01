#pragma once

class IRequest {
 public:
  virtual ~IRequest() {}
  void parseRequest(void);
  void matchServerConf(short port);
  void validatePath(void);
};