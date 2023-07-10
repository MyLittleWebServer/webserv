#pragma once

#include <unistd.h>

#include <iostream>

#include "Client.hpp"
#include "Config.hpp"
#include "ExceptionThrower.hpp"
#include "ICGI.hpp"
#include "Request.hpp"

/**
 * @brief CGI 인터페이스
 * @author middlefitting
 * @date 2023.07.05
 */

class CGI : public ICGI {
 private:
  std::string _cgiResult;
  bool _excuteFlag;
  bool _finishFlag;
  pid_t _pid;
  int _in_pipe[2];
  int _out_pipe[2];
  IRequest* request;
  char** _env;

 private:
  CGI();
  CGI(const CGI& copy);
  CGI& operator=(const CGI& copy);

  void excuteCgi();
  void waitAndRead();
  void waitChild();
  void readChild();

 public:
  CGI(IRequest* request);
  ~CGI();

  void execute();
  const std::string& getCgiResult();
};