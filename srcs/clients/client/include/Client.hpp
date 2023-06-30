#ifndef Client_HPP
#define Client_HPP

#include <fcntl.h>
#include <sys/socket.h>
#include <unistd.h>

#include <iostream>
#include <map>
#include <string>

#include "AMethod.hpp"
#include "DELETE.hpp"
#include "DummyMethod.hpp"
#include "GET.hpp"
#include "POST.hpp"

#define RECEIVE_LEN 1000

enum ClientFlag {
  START,
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

class Client {
 private:
  ClientFlag _flag;
  uintptr_t _sd;
  std::string _request;

  static char _buf[RECEIVE_LEN + 1];

  bool checkIfReceiveFinished(ssize_t n);
  // std::map<uintptr_t, char *> _clientBuf;

 public:
  AMethod *_method;
  Client();
  Client(const uintptr_t sd);
  Client &operator=(const Client &src);
  virtual ~Client();

 public:
  uintptr_t getSD() const;
  AMethod *getMethod() const;
  void receiveRequest();
  void newHTTPMethod();
  void sendResponse();
  void setFlag(ClientFlag flag);
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
