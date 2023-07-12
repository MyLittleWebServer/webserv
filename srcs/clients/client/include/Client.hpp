#ifndef Client_HPP
#define Client_HPP

#include <fcntl.h>
#include <sys/socket.h>
#include <unistd.h>

#include <iostream>
#include <map>
#include <string>

#include "CGI.hpp"
#include "ICGI.hpp"
#include "IMethod.hpp"
#include "Request.hpp"
#include "Response.hpp"

#define RECEIVE_LEN 1000

enum ClientFlag {
  START,
  RECEIVING,
  RECEIVE_DONE,
  REQUEST_HEAD,
  REQUEST_ENTITY,
  REQUEST_DONE,
  FILE_READ,
  FILE_CGI,
  FILE_WRITE,
  FILE_DONE,
  RESPONSE_DONE,
  END
};

class Utils;

class Client {
 private:
  ClientFlag _flag;
  uintptr_t _sd;
  std::string _recvBuff;
  Request _request;
  Response _response;
  IMethod *_method;
  ICGI *_cgi;

  static char _buf[RECEIVE_LEN + 1];

  bool checkIfReceiveFinished(ssize_t n);
  // std::map<uintptr_t, char *> _clientBuf;

 public:
  Client();
  Client(const uintptr_t sd);
  Client &operator=(const Client &src);
  virtual ~Client();

 public:
  uintptr_t getSD() const;
  IMethod *getMethod() const;
  void receiveRequest();
  void newHTTPMethod();
  void sendResponse();
  void setFlag(ClientFlag flag);
  void parseRequest(short port);
  bool isCgi();
  void doRequest();
  void createErrorResponse();
  void createSuccessResponse();
  void makeAndExecuteCgi();
  ClientFlag getFlag() const;
  class RecvFailException : public std::exception {
   public:
    const char *what() const throw();
  };
  class DisconnectedDuringRecvException : public std::exception {
   public:
    const char *what() const throw();
  };

  class SendFailException : public std::exception {
   public:
    const char *what() const throw();
  };
  class DisconnectedDuringSendException : public std::exception {
   public:
    const char *what() const throw();
  };
};

std::ostream &operator<<(std::ostream &os, const Client &client);

#endif
