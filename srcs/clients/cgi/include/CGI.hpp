#pragma once

#include <unistd.h>

#include <iostream>

#include "Client.hpp"
#include "Config.hpp"
#include "ExceptionThrower.hpp"
#include "ICGI.hpp"
#include "Kqueue.hpp"
#include "Request.hpp"
#include "Response.hpp"

/**
 * @brief CGI 인터페이스
 * @author middlefitting
 * @date 2023.07.05
 */

class CGI : public ICGI {
 private:
  uintptr_t _client_fd;
  std::string _cgiResult;
  bool _excuteFlag;
  bool _waitFinishFlag;
  bool _cgiFinishFlag;
  pid_t _pid;
  int _in_pipe[2];
  int _out_pipe[2];
  IRequest* _request;
  IResponse* _response;
  std::vector<const char*> _env;
  void* _client_info;
  std::string _body;

 private:
  CGI();
  CGI(const CGI& copy);
  CGI& operator=(const CGI& copy);

  void initEnv();

  void setFcntl(int fd);
  void setPipeNonblock();
  void excuteCgi();

  void makeChild();

  void waitChild();

  bool readChildFinish();

  void setPipeNonblock();
  void setFcntl(int fd);

  void generateErrorResponse(Status status);

 public:
  CGI(IRequest* request, IResponse* response, uintptr_t client_fd,
      void* client_info);
  ~CGI();

  void executeCGI();
  void writeCGI();
  void waitAndReadCGI();

  const std::string& getCgiResult();
  bool isCgiFinish();
};
