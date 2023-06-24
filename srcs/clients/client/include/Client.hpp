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

class Client {
 private:
  const uintptr_t _sd;
  std::string _request;
  AMethod *_method;

  static char _buf[RECEIVE_LEN + 1];

  bool checkIfReceiveFinished(ssize_t n) const;

 public:
  Client();
  Client(const uintptr_t sd);
  virtual ~Client();

 public:
  AMethod *getMethod() const;
  void receiveRequest();
  void newHTTPMethod();
  void sendResponse(std::map<int, Client> &_clients);

  class RecvFailException : public std::exception {
   public:
    const char *what() const throw();
  };
  class DisconnectedDuringRecvException : public std::exception {
   public:
    const char *what() const throw();
  };
};

#endif
