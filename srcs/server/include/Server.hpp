#ifndef Server_HPP
#define Server_HPP

#include <netinet/in.h>
#include <sys/socket.h>

#define WAITLISTSIZE 10

class Server {
  typedef struct sockaddr_in sAddr_t;

 private:
  sAddr_t _addr;
  int _socket;
  int _waitListSize;
  int _port;

 public:
  Server(void);
  Server(int port);
  virtual ~Server(void);

 public:
  uintptr_t getSocket(void) const;

 public:
  void socketInit(void);
  void addrInit(void);
  void bindSocketWithAddr(void);
  void listenSocket(void) const;
  void asyncSocket(void) const;
  void startServer(void);
};

#endif