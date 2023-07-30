#pragma once

#include <netinet/in.h>
#include <sys/socket.h>

#include <set>
#include <string>
#include <vector>

#define BACKLOG 128

/**
 * @brief 서버의 정보를 담고있는 클래스
 *
 * @details
 * Server 클래스는 소켓을 초기화하고, 주소를 초기화합니다.
 * 그리고 소켓과 주소를 바인딩하고, 소켓을 리스닝합니다.
 *
 * @see _addr : Socket address, internet style의 서버의 주소를
 * 담고있습니다.
 * @see _socket : 서버의 소켓(FD)을 담고있습니다.
 * @see _port : 서버의 포트를 담고있습니다.
 * @see _host : 서버의 호스트를 담고있습니다.
 *
 * @author chanhihi
 * @date 2023.07.17
 */
class Server {
  typedef struct sockaddr_in sAddr_t;

 private:
  sAddr_t _addr;
  int _socket;
  int _port;
  std::string _host;

  void hostInit(void);
  void socketInit(void);
  void addrInit(void);
  void bindSocketWithAddr(void);
  void listenSocket(void) const;
  void asyncSocket(void) const;

 public:
  Server(int port);
  Server(const Server& src);
  Server& operator=(const Server& src);
  virtual ~Server(void);

 public:
  uintptr_t getSocket(void) const;
  short getPort(void) const;
  std::string getHost(void) const;

 public:
  void initServerSocket(void);
};
