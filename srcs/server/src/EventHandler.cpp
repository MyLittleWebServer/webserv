/**
 * @file EventHandler.cpp
 * @brief 서버의 이벤트를 처리하는 소스파일입니다.
 * @author chanhihi
 * @date 2023-07-20
 * @copyright Copyright (c) 2023
 */

#include "EventHandler.hpp"

#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

/**
 * @brief EventHandler 생성자
 *
 * @details
 * Event Handler 생성자입니다.
 * 중복된 서버소켓의 처리를 위해 서버의 소켓을 _serverSocketSet에 저장합니다.
 * 해당 소켓의 ident로만 이벤트처리가 가능하므로. Set에 서버소켓이 저장되면
 * serverVector의 Server 객체를 삭제합니다.
 *
 * @param serverVector
 *
 * @return EventHandler
 *
 * @author chanhihi
 * @date 2023-07-21
 */
EventHandler::EventHandler(const std::vector<Server *> &serverVector)
    : _errorFlag(false) {
  for (std::vector<Server *>::const_iterator it = serverVector.begin();
       it != serverVector.end(); ++it) {
    _serverSocketSet.insert((*it)->getSocket());
    delete (*it);
  }
}

/**
 * @brief Event Handler 소멸자
 *
 * @author chanhihi
 * @date 2023-07-21
 */
EventHandler::~EventHandler(void) {}

/**
 * @brief 현재 이벤트를 설정합니다.
 *
 * @details
 * 이전에 newEvents() 함수에서 kevent를 호출하여 받아온 eventList의 index번째
 * 이벤트를 _currentEvent로 설정합니다.
 *
 * @param i
 *
 * @author chanhihi
 * @date 2023-07-17
 */
void EventHandler::setCurrentEvent(int i) { _currentEvent = &(_eventList[i]); }

/**
 * @brief checkFlags : kevent의 flags를 확인합니다.
 *
 * @details
 * currentEvent의 flags를 확인하여 에러가 발생했는지 확인합니다.
 * 에러가 발생했다면 checkErrorOnSocket() 함수를 호출합니다.
 *
 * @see checkErrorOnSocket()
 *
 * @author chanhihi
 * @date 2023-07-21
 */
void EventHandler::checkFlags(void) {
  _errorFlag = false;
  if (_currentEvent->flags & EV_ERROR) {
    _errorFlag = true;
    checkErrorOnSocket();
  }
  if (_currentEvent->flags & EV_EOF &&
      Kqueue::getFdType(_currentEvent->ident) == FD_CLIENT) {
    _errorFlag = true;
    disconnectClient(static_cast<Client *>(_currentEvent->udata));
  }
}

/**
 * @brief branchCondition : currentEvent의 ident를 기반으로 분기합니다.
 *
 * @details
 * ident가 서버소켓이라면 acceptClient() 함수를 호출합니다.
 * ident가 클라이언트 소켓이라면 clientCondition() 함수를 호출합니다.
 * ident가 CGI 소켓이라면 cgiCondition() 함수를 호출합니다.
 *
 * @see acceptClient()
 * @see clientCondition()
 * @see cgiCondition()
 * @see Kqueue::getFdType()
 *
 * @author chanhihi
 * @date 2023-07-17
 */
void EventHandler::branchCondition(void) {
  if (_errorFlag == true) {
    return;
  }
  e_fd_type fd_type = Kqueue::getFdType(_currentEvent->ident);
  switch (fd_type) {
    case FD_SERVER: {
      acceptClient();
      break;
    }
    case FD_CLIENT: {
      clientCondition();
      break;
    }
    case FD_CGI:
      cgiCondition();
      break;
    default:
      break;
  }
}

/**
 * @brief 클라이언트의 연결을 수락합니다.
 *
 * @details
 * accept() 함수는 서버소켓에 대한 연결 요청을 수락합니다.
 *
 * @see accept : 클라이언트의 연결을 수락하는 함수
 *
 * @exception accept() error : accept() 함수가 에러를 반환하면 예외를 던집니다.
 * @exception accept fcntl error : fcntl() 함수가 에러를 반환하면 예외를
 * 던집니다.
 *
 * @author chanhihi
 * @date 2023-07-17
 */
void EventHandler::acceptClient() {
  uintptr_t clientSocket;
  if ((clientSocket = accept(_currentEvent->ident, NULL, NULL)) == -1) {
    throwWithPerror("accept() error\n" + std::string(strerror(errno)));
  }
  std::cout << "accept ... : " << clientSocket << std::endl;
  fcntl(clientSocket, F_SETFL, O_NONBLOCK);
  registClient(clientSocket);
}

/**
 * @brief 클라이언트 소켓의 이벤트를 등록합니다.
 *
 * @details
 * 클라이언트 소켓의 이벤트는 read, write입니다.
 * EVFILT_READ는 클라이언트로부터 요청(READ EVENT)을 받기 위한 이벤트입니다.
 * EVFILT_WRITE는 클라이언트에게 응답(WRITE EVENT)을 하기 위한 이벤트입니다.
 * READ EVENT는 ENABLE로 등록하여 클라이언트로부터 요청을 받을 수 있도록
 * 합니다. WRITE EVENT는 초기에 DISABLE로 등록하여 클라이언트에게 응답을 하지
 * 않도록 합니다. 클라이언트의 요청을 받은 후에 응답을 하기 위해 ENABLE로
 * 변경합니다.
 *
 * @see EVFILT_READ
 * @see EVFILT_WRITE
 * @see EV_ADD
 * @see EV_ENABLE
 *
 * @param clientSocket
 */
void EventHandler::registClient(const uintptr_t clientSocket) {
  Client *newClient = new Client(clientSocket);
  addEvent(clientSocket, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0,
           static_cast<void *>(newClient));
  addEvent(clientSocket, EVFILT_WRITE, EV_ADD | EV_DISABLE, 0, 0,
           static_cast<void *>(newClient));
  setFdSet(clientSocket, FD_CLIENT);
}

/**
 * @brief 클라이언트의 연결을 해제합니다.
 *
 * @details
 * 1. 클라이언트의 write, read 이벤트를 삭제합니다.
 * 2. 클라이언트의 fd_set을 삭제합니다.
 *
 * @param client
 */
void EventHandler::disconnectClient(Client *client) {
  deleteEvent((uintptr_t)client->getSD(), EVFILT_WRITE,
              static_cast<void *>(client));
  deleteEvent((uintptr_t)client->getSD(), EVFILT_READ,
              static_cast<void *>(client));
  deleteFdSet((uintptr_t)client->getSD(), FD_CLIENT);
  std::cout << "Client " << client->getSD() << " disconnected!" << std::endl;
  delete client;
}

/**
 * @brief 소켓에 에러가 발생했는지 확인합니다.
 *
 * @details
 * serverSocketSet에 현재 이벤트의 ident가 존재한다면 서버소켓에 에러가 발생한
 * 것입니다. 서버소켓에 에러가 발생하면 예외를 발생시킵니다.
 * 만약 ident가 존재하지 않는다면 client socket에 에러가 발생한 것이므로, 해당
 * 클라이언트의 접속을 끊습니다.
 *
 * @exception throw : 서버소켓에 에러가 발생하면 예외를 발생시킵니다.
 *
 * @author chanhihi
 * @date 2023-07-21
 */
void EventHandler::checkErrorOnSocket() {
  if (_serverSocketSet.find(_currentEvent->ident) != _serverSocketSet.end()) {
    throwWithPerror("server socket error");
  }
  std::cout << " client socket error" << std::endl;
  disconnectClient(static_cast<Client *>(_currentEvent->udata));
}

/**
 * @brief currentEvent의 filter 기반 Client로 분기하는 함수.
 *
 * @details
 * 현재 이벤트의 udata를 Client로 캐스팅하여 currClient에 저장합니다.
 * currClient
 *
 * filter가 EVFILT_READ라면 processRequest() 함수를 호출합니다.
 * filter가 EVFILT_WRITE라면 processResponse() 함수를 호출합니다.
 * filter가 EVFILT_TIMER라면 processTimeOut() 함수를 호출합니다.
 *
 */
void EventHandler::clientCondition() {
  Client &currClient = *(static_cast<Client *>(_currentEvent->udata));
  if (_currentEvent->filter == EVFILT_READ) {
    processRequest(currClient);
  } else if (_currentEvent->filter == EVFILT_WRITE) {
    processResponse(currClient);
  } else if (_currentEvent->filter == EVFILT_TIMER) {
    processTimeOut(currClient);
  }
}

/**
 * @brief currentEvent의 filter 기반 CGI로 분기하는 함수.
 *
 * @details
 * 현재 이벤트의 udata를 ICGI로 캐스팅하여 cgi에 저장합니다.
 * filter가 EVFILT_READ라면 waitAndReadCGI() 함수를 호출합니다.
 * filter가 EVFILT_WRITE라면 writeCGI() 함수를 호출합니다.
 */
void EventHandler::cgiCondition() {
  ICGI &cgi = *(static_cast<ICGI *>(_currentEvent->udata));
  if (_currentEvent->filter == EVFILT_READ) {
    cgi.waitAndReadCGI();
  } else if (_currentEvent->filter == EVFILT_WRITE) {
    cgi.writeCGI();
  }
}

/**
 * @brief READ Event로 들어온 Request를 처리합니다.
 *
 * @details
 * 클라이언트로부터 READ Event를 수신할 때 호출됩니다.
 * 1. 현재 클라이언트의 상태가 RECEIVING이라면, 클라이언트의 kqueue에서 타임아웃
 * 이벤트를 제거합니다.
 * 2. 클라이언트 요청 후에 등록 될 요청타임아웃 이벤트를 eventsToAdd에
 * 등록합니다.
 * 3. 클라이언트로부터 요청을 수신합니다.
 * 4. 클라이언트의 요청을 현재이벤트의 Port기준으로 파싱합니다.
 * 5. 클라이언트의 상태가 REQUEST_DONE이라면 eventsToAdd에서 타임아웃 이벤트를
 * 제거합니다.
 * 6. 클라이언트의 상태가 CGI라면 makeAndExecuteCgi() 함수를 호출합니다.
 * 7. 클라이언트의 상태가 CGI가 아니라면 어느 Method인지 선택합니다.
 * 8. 클라이언트는 Request를 처리합니다.
 * 9. 클라이언트는 성공 응답을 생성합니다.
 * 10. 해당 클라이언트의 SD를 통해서 READ 이벤트를 비활성화하고, WRITE 이벤트를
 * 활성화 합니다.
 *
 * @see GET
 * @see POST
 * @see PUT
 * @see DELETE
 *
 * @param currClient
 *
 * @exception code : status code가 catch되면 예외 응답을 생성합니다.
 * @exception std::exception : status code가 아닌 예외가 발생하면 클라이언트를
 * 연결을 끊습니다.
 */
void EventHandler::processRequest(Client &currClient) {
  try {
    if (currClient.getState() == START) {
      registTimerEvent();
    }
    std::cout << "socket descriptor : " << currClient.getSD() << std::endl;
    currClient.receiveRequest();
    currClient.parseRequest(getBoundPort(_currentEvent));
    if (currClient.getState() == EXPECT_CONTINUE) {
      currClient.createContinueResponse();
      enableEvent(currClient.getSD(), EVFILT_WRITE,
                  static_cast<void *>(&currClient));
      disableEvent(currClient.getSD(), EVFILT_READ,
                   static_cast<void *>(&currClient));
      return;
    }
    if (currClient.getState() == RECEIVING) {
      return;
    }
    deleteTimerEvent();
    if (currClient.isCgi()) {
      currClient.makeAndExecuteCgi();
    } else {
      enactRequestAndCreateResponse(currClient);
    }
  } catch (enum Status &code) {
    handleExceptionStatusCode(currClient);
  } catch (std::exception &e) {
    disconnectClient(&currClient);
    std::cerr << e.what() << '\n';
    return;
  }
}

void EventHandler::registTimerEvent() {
  registEvent(_currentEvent->ident, EVFILT_TIMER, EV_ADD | EV_ONESHOT,
              NOTE_SECONDS, 60, static_cast<void *>(_currentEvent->udata));
}

void EventHandler::deleteTimerEvent() {
  deleteEvent(_currentEvent->ident, EVFILT_TIMER,
              static_cast<void *>(_currentEvent->udata));
}

void EventHandler::enactRequestAndCreateResponse(Client &currClient) {
  currClient.newHTTPMethod();
  currClient.doRequest();
  currClient.createSuccessResponse();
  disableEvent(currClient.getSD(), EVFILT_READ,
               static_cast<void *>(&currClient));
  enableEvent(currClient.getSD(), EVFILT_WRITE,
              static_cast<void *>(&currClient));
}

void EventHandler::handleExceptionStatusCode(Client &currClient) {
  if (currClient.getState() == RECEIVING) {
    deleteTimerEvent();
  }
  currClient.createExceptionResponse();
  disableEvent(currClient.getSD(), EVFILT_READ,
               static_cast<void *>(&currClient));
  enableEvent(currClient.getSD(), EVFILT_WRITE,
              static_cast<void *>(&currClient));
}

/**
 * @brief WRITE Event로 들어온 Response를 처리합니다.
 *
 * @details
 * 클라이언트로부터 WRITE Event를 수신할 때 호출됩니다.
 * 1. 현재 클라이언트의 상태가 PROCESS_RESPONSE가 아니라면,
 * 클라이언트의 setResponseConnection 함수를 호출합니다.
 * 2. 클라이언트 응답을 전송합니다.
 * 3. 전송중 에러가 발생하면 현재 클라이언트의 연결을 끊습니다.
 * 4. 클라이언트의 상태가 END_KEEP_ALIVE라면 클라이언트의 WRITE 이벤트를
 * 비활성화하고, READ 이벤트를 활성화합니다.
 * 5. 클라이언트의 상태가 END_CLOSE라면 클라이언트의 연결을 끊습니다.
 *
 * @param currClient
 */
void EventHandler::processResponse(Client &currClient) {
  if (currClient.getState() != PROCESS_RESPONSE) {
    currClient.bodyCheck();
    currClient.setResponseConnection();
    currClient.reassembleResponse();
  }
  try {
    currClient.sendResponse();
  } catch (std::exception &e) {
    std::cerr << e.what() << '\n';
    disconnectClient(&currClient);
    return;
  }
  validateConnection(currClient);
}

void EventHandler::validateConnection(Client &currClient) {
  if (currClient.getState() == RECEIVING) {
    enableEvent(currClient.getSD(), EVFILT_READ,
                static_cast<void *>(&currClient));
  }
  if (currClient.getState() == END_KEEP_ALIVE) {
    disableEvent(currClient.getSD(), EVFILT_WRITE,
                 static_cast<void *>(&currClient));
    enableEvent(currClient.getSD(), EVFILT_READ,
                static_cast<void *>(&currClient));
    currClient.clear();
  } else if (currClient.getState() == END_CLOSE) {
    disconnectClient(&currClient);
  }
}

void EventHandler::processTimeOut(Client &currClient) {
  currClient.setConnectionClose();
  currClient.createExceptionResponse(E_408_REQUEST_TIMEOUT);
  disableEvent(currClient.getSD(), EVFILT_WRITE,
               static_cast<void *>(&currClient));
  enableEvent(currClient.getSD(), EVFILT_WRITE,
              static_cast<void *>(&currClient));
}
