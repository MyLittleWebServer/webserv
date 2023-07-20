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

#define RECEIVE_LEN 1460

enum ClientStates {
  START,
  RECEIVING,

  REQUEST_HEAD,
  REQUEST_ENTITY,

  METHOD_SELECT,

  PROCESS_RESPONSE,
  FILE_READ,
  FILE_CGI,
  FILE_WRITE,
  FILE_DONE,
  RESPONSE_DONE,
  END_KEEP_ALIVE,
  END_CLOSE
};

class Utils;

class Client {
 private:
  ClientStates _state;
  uintptr_t _sd;
  std::string _recvBuff;
  Request _request;
  Response _response;
  IMethod *_method;
  ICGI *_cgi;
  size_t _lastSentPos;

  static char _buf[RECEIVE_LEN];

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
  void setState(ClientStates state);
  void parseRequest(short port);
  bool isCgi();
  void doRequest();
  void createExceptionResponse();
  void createExceptionResponse(Status statusCode);
  void createSuccessResponse();
  void makeAndExecuteCgi();
  void clear();
  void setResponseConnection();
  void setConnectionClose();

  ClientStates getState() const;
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
