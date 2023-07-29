#ifndef EventHandler_HPP
#define EventHandler_HPP

#include <sys/event.h>

#include <set>

#include "Client.hpp"
#include "Kqueue.hpp"
#include "Server.hpp"

class Client;

/**
 * @brief EventHandler class : 서버의 이벤트를 관리하는 클래스입니다.
 *
 * @details
 * EventHandler는 kevent와 FD를 관리하는 Kqueue로 부터 상속 받은 클래스입니다.
 * EventHandler 안에서 kevent를 관리하고, kevent를 통해 이벤트를 처리합니다.
 * EventHandler는 다음과 같은 이벤트를 처리합니다.
 *
 * 1. 클라이언트의 연결 요청
 * 2. 클라이언트의 연결 종료
 * 3. 클라이언트의 요청
 * 4. 클라이언트의 응답
 * 5. 클라이언트의 타임아웃
 * 6. 클라이언트 상태
 * 7. CGI 상태
 * 8. 소켓 에러 상태
 *
 * @see Kqueue
 * @see Client
 * @see Server
 * @see kevent
 * @see FD
 *
 * @author chanhihi
 * @date 2023.07.17
 */
class EventHandler : public Kqueue {
 private:
  struct kevent* _currentEvent;
  bool _errorFlag;

  void acceptClient(void);
  void registClient(const uintptr_t clientSocket);
  void disconnectClient(Client* client);

  void checkErrorOnSocket(void);

  void clientCondition();
  void cgiCondition();
  void timerCondition();

  void processRequest(Client& client);
  void enactRequestAndCreateResponse(Client& client);
  void handleExceptionStatusCode(Client& client);

  void processResponse(Client& client);
  void validateConnection(Client& client);

  void processTimeOut(Client& client);
  void processRequestTimeOut(Client& client);
  void processKeepAliveTimeOut(Client& client);

  void setKeepAliveTimeOutTimer(Client& clinet);
  void setRequestTimeOutTimer(Client& clinet);
  void deleteTimerEvent();

 public:
  EventHandler();
  virtual ~EventHandler();

  void setCurrentEvent(int i);
  void checkFlags(void);
  void branchCondition(void);
};

#endif
