#ifndef Client_HPP
#define Client_HPP

#include <fcntl.h>
#include <sys/socket.h>
#include <unistd.h>

#include <iostream>
#include <map>
#include <string>

#include "./Clients/AMethod.hpp"
#include "./Clients/DELETE.hpp"
#include "./Clients/GET.hpp"
#include "./Clients/POST.hpp"

#define TRANSFER_LEN 10

class Client {
 private:
  const uintptr_t _sd;
  std::string _request;
  AMethod *_method;

  static char _buf[TRANSFER_LEN + 1];

 public:
  Client(void);
  Client(const uintptr_t sd);
  virtual ~Client(void);

 public:
  void receiveData(void);
  void makeResponse(void);
  void sendData(std::map<int, Client> &_clients);
  void newHTTPMethod(void);
};

#endif
