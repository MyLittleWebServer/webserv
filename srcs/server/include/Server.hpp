#pragma once

#include <netinet/in.h>
#include <sys/socket.h>

#include <set>
#include <vector>

#define BACKLOG 128

class Server {
  typedef struct sockaddr_in sAddr_t;

 private:
  sAddr_t _addr;
  int _socket;
  int _port;

  void socketInit(void);
  void addrInit(void);
  void bindSocketWithAddr(void);
  void listenSocket(void) const;
  void asyncSocket(void) const;

 public:
  Server(int port);
  virtual ~Server(void);

 public:
  uintptr_t getSocket(void) const;
  short getPort(void) const;

 public:
  void initServerSocket(void);
};
