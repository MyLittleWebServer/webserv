#include "Server.hpp"

#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/event.h>
#include <sys/types.h>

#include <cstdint>
#include <string>

#include "Config.hpp"
#include "Utils.hpp"

Server::Server(int port) : _port(port) {}

Server::~Server(void) {}

/**
 * @brief initServerSocket 서버 소켓을 초기화합니다.
 *
 * @details
 * 1. hostInit() : 서버의 host(port)를 config에서 가져옵니다.
 * 2. socketInit() : 서버의 소켓(socket FD)을 초기화합니다.
 * 3. addrInit() : 서버의 주소(port)를 초기화합니다.
 * 4. bindSocketWithAddr() : 서버의 소켓과 주소를 바인딩합니다.
 * 5. listenSocket() : 서버의 소켓을 listen 상태로 변경합니다.
 * 6. asyncSocket() : 서버의 소켓을 비동기 상태로 변경합니다.
 *
 * @author chanhihi
 * @date 2023.07.17
 */
void Server::initServerSocket(void) {
  this->hostInit();
  this->socketInit();
  this->addrInit();
  this->bindSocketWithAddr();
  this->listenSocket();
  this->asyncSocket();
}

/**
 * @brief hostInit 서버의 host(port)를 config에서 가져옵니다.
 *
 * @details
 * Config에서 서버의 host(port)를 가져옵니다.
 * 만약 config에 서버의 host(port)가 없다면, ""로 설정합니다.
 *
 * @see Config
 * @see Config::getInstance
 * @see Config::getServerConfigs
 * @see IServerConfig
 * @see IServerConfig::getListen
 * @see IServerConfig::getServerName
 *
 * @author chanhihi
 * @date 2023.07.17
 */
void Server::hostInit(void) {
  Config &config = Config::getInstance();
  std::list<IServerConfig *> serverInfo = config.getServerConfigs();
  std::list<IServerConfig *>::iterator it = serverInfo.begin();
  while (it != serverInfo.end()) {
    if ((*it)->getListen() == this->_port) {
      this->_host = (*it)->getServerName();
      return;
    } else {
      this->_host = "";
    }
    ++it;
  }
}

/**
 * @brief socketInit 서버의 소켓(socket FD)을 초기화합니다.
 *
 * @details
 * socket() 함수를 호출하여 서버의 소켓을 초기화합니다.
 *
 * @see socket : 소켓을 생성하는 함수
 * @see PF_INET : IPv4 인터넷 프로토콜
 * @see SOCK_STREAM : TCP
 *
 * @exception throwWithPerror socket() 함수가 실패하면 예외를 던집니다.
 *
 * @author chanhihi
 * @date 2023.07.17
 */
void Server::socketInit(void) {
  this->_socket = socket(PF_INET, SOCK_STREAM, 0);
  if (this->_socket == -1) throwWithPerror("socket() error");
}

/**
 * @brief addrInit 서버의 주소(port)를 초기화합니다.
 *
 * @details
 * 1. std::memset() 함수를 호출하여 _addr을 0으로 초기화합니다.
 * 2. _addr의 sin_family를 AF_INET으로 설정합니다.
 * 3. _addr의 sin_addr.s_addr를 htonl(INADDR_ANY)로 설정합니다.
 * 4. _addr의 sin_port를 htons(_port)로 설정합니다.
 *
 * @see AF_INET : IPv4 인터넷 프로토콜
 * @see htonl : 호스트 바이트 순서를 네트워크 바이트 순서로 변환하는 함수
 * @see INADDR_ANY : 모든 IP 주소를 나타내는 상수
 * @see htons : 호스트 바이트 순서를 네트워크 바이트 순서로 변환하는 함수
 * @see _port : 서버의 포트
 * @see _addr : 서버의 주소
 * @see _addr.sin_family : 주소 체계
 * @see _addr.sin_addr.s_addr : IP 주소
 * @see _addr.sin_port : 포트 번호
 *
 * @author chanhihi
 * @date 2023.07.17
 */
void Server::addrInit(void) {
  std::memset(&_addr, 0, sizeof(_addr));
  _addr.sin_family = AF_INET;
  _addr.sin_addr.s_addr = htonl(INADDR_ANY);
  _addr.sin_port = htons(_port);
}

/**
 * @brief bindSocketWithAddr 서버의 소켓과 주소를 바인딩합니다.
 *
 * @details
 * 1. setsockopt() 함수를 호출하여 SO_REUSEADDR 옵션을 설정합니다.
 * 2. bind() 함수를 호출하여 서버의 소켓과 주소를 바인딩합니다.
 *
 * @see setsockopt : 소켓 옵션을 설정하는 함수
 * @see SO_REUSEADDR : TIME_WAIT 상태의 포트를 재사용하는 옵션
 * @see bind : 소켓에 주소를 할당하는 함수
 *
 * @exception throwWithPerror setsockopt() 함수가 실패하면 예외를 던집니다.
 * @exception throwWithPerror bind() 함수가 실패하면 예외를 던집니다.
 *
 * @author chanhihi
 * @date 2023.07.17
 */
void Server::bindSocketWithAddr(void) {
  int enable = 1;
  if (setsockopt(this->_socket, SOL_SOCKET, SO_REUSEADDR, &enable,
                 sizeof(int)) < 0) {
    throwWithPerror("setsockopt(SO_REUSEADDR) failed");
  }

  if (bind(this->_socket, (struct sockaddr *)&_addr, sizeof(_addr)) == -1)
    throwWithPerror("bind() error");
}

/**
 * @brief listenSocket 서버의 소켓을 listen 상태로 변경합니다.
 *
 * @details
 * listen() 함수는 생성된 소켓이 클라이언트로부터 연결 요청을 대기하기
 * 시작하도록 하는 역할을 합니다. 이 함수는 두가지의 인자를 받습니다. socket과
 * BACKLOG입니다. socket은 소켓 디스크립터를 의미하며, BACKLOG는 서버가 동시에
 * 대기할 수 있는 최대 연결 요청의 수를 의미합니다. BACKLOG로 설정된 수보다 많은
 * 클라이언트가 연결 요청을 하면, 서버는 연결 요청을 거절되거나 대기열이 여유
 * 공간이 생길 때 까지 대기하게 됩니다.
 *
 * @see listen : 소켓을 수동 대기 상태로 변경하는 함수
 * @see BACKLOG : 대기 큐의 크기
 *
 * @author chanhihi
 * @date 2023.07.17
 */
void Server::listenSocket(void) const {
  if (listen(this->_socket, BACKLOG) == -1) throwWithPerror("listen() error");
}

/**
 * @brief asyncSocket 서버의 소켓을 비동기 상태로 변경합니다.
 *
 * @details
 * fcntl() 함수를 호출하여 서버의 소켓을 비동기 상태로 변경합니다.
 *
 * @see fcntl : 디스크립터에 대한 저수준 조작을 수행하는 함수
 * @see _socket : 서버의 소켓
 * @see F_SETFL : 디스크립터의 상태 플래그를 설정하는 명령
 * @see O_NONBLOCK : 디스크립터를 논블록 모드로 설정하는 플래그
 *
 * @author chanhihi
 * @date 2023.07.17
 */
void Server::asyncSocket(void) const {
  if (fcntl(this->_socket, F_SETFL, O_NONBLOCK) == -1)
    throwWithPerror("fcntl() error");
}

/**
 * @brief getSocket 서버의 소켓을 반환합니다.
 * @return uintptr_t
 */
uintptr_t Server::getSocket(void) const { return (this->_socket); }

/**
 * @brief getPort 서버의 포트를 반환합니다.
 * @return short
 */
short Server::getPort(void) const { return (this->_port); }

/**
 * @brief getHost 서버의 host를 반환합니다.
 * @return std::string
 */
std::string Server::getHost(void) const { return (this->_host); }
