#pragma once

#include <unistd.h>

#include <iostream>

#include "Client.hpp"
#include "Config.hpp"
#include "ExceptionThrower.hpp"
#include "ICGI.hpp"
#include "Kqueue.hpp"
#include "Request.hpp"

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
  bool _finishFlag;
  pid_t _pid;
  int _in_pipe[2];
  int _out_pipe[2];
  IRequest* _request;
  std::vector<const char*> _env;

 private:
  CGI();
  CGI(const CGI& copy);
  CGI& operator=(const CGI& copy);

  void initEnv();

  void excuteCgi();
  void waitAndRead();
  void waitChild();
  void readChild();

  void setPipeNonblock();
  void setFcntl(int fd);

 public:
  CGI(IRequest* request, uintptr_t client_fd);
  ~CGI();

  void execute();
  const std::string& getCgiResult();
};
