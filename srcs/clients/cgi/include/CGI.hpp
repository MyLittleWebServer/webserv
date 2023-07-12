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
  std::string _content_type;
  std::string _content_length;
  std::string _http_user_agent;
  std::string _server_name;
  std::string _server_software;
  std::string _path_info;
  std::string _query_string;
  std::string _remote_addr;
  std::string _request_method;
  std::string _script_filename;

 private:
  uintptr_t _client_fd;
  std::string _cgiResult;
  bool _executeFlag;
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
  void execute();

  void makeChild();

  void waitChild();

  bool readChildFinish();

  void generateErrorResponse(Status status);
  void generateResponse();

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
