
#include "EventHandler.hpp"

#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

/**
 * @brief Event Handler 생성자
 *
 * @details
 * Event Handler 생성자입니다.
 * 중복된 서버소켓의 처리를 위해 서버의 소켓을 _serverSocketSet에 저장합니다.
 * 해당 소켓의 ident로만 이벤트처리가 가능하므로. Set에 서버소켓이 저장되면
 * serverVector의 Server 객체를 삭제합니다.
 *
 * @param serverVector
 *
 * @author chanhihi
 * @date 2023.07.17
 */
EventHandler::EventHandler(const std::vector<Server *> &serverVector)
    : _errorFlag(false) {
  for (std::vector<Server *>::const_iterator it = serverVector.begin();
       it != serverVector.end(); ++it) {
    this->_serverSocketSet.insert((*it)->getSocket());
    delete (*it);
  }
}

EventHandler::~EventHandler(void) {}

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
 * @date 2023.07.17
 */
void EventHandler::checkFlags(void) {
  this->_errorFlag = false;
  if (_currentEvent->flags & EV_ERROR) {
    this->_errorFlag = true;
    checkErrorOnSocket();
  }
  if (_currentEvent->flags & EV_EOF &&
      Kqueue::getFdType(_currentEvent->ident) == FD_CLIENT) {
    _errorFlag = true;
    disconnectClient(static_cast<Client *>(_currentEvent->udata));
  }
}

void EventHandler::checkErrorOnSocket() {
  if (_serverSocketSet.find(this->_currentEvent->ident) !=
      this->_serverSocketSet.end())
    throwWithPerror("server socket error");
  std::cout << " client socket error" << std::endl;
  disconnectClient(static_cast<Client *>(this->_currentEvent->udata));
}

/**
 * @brief setCurrentEvent : 현재 이벤트를 설정합니다.
 *
 * @details
 * 이전에 newEvents() 함수에서 kevent를 호출하여 받아온 eventList의 index번째
 * 이벤트를 _currentEvent로 설정합니다.
 *
 * @param i
 *
 * @author chanhihi
 * @date 2023.07.17
 */
void EventHandler::setCurrentEvent(int i) {
  this->_currentEvent = &(this->_eventList[i]);
}

/**
 * @brief currentEvent의 filter를 기반으로 분기합니다.
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
void EventHandler::clientCondtion() {
  Client &currClient = *(static_cast<Client *>(this->_currentEvent->udata));
  if (this->_currentEvent->filter == EVFILT_READ) {
    processRequest(currClient);
  } else if (this->_currentEvent->filter == EVFILT_WRITE) {
    processResponse(currClient);
  } else if (this->_currentEvent->filter == EVFILT_TIMER) {
    // create timeout response
    std::cout << "TIMEOUT OCCURRED!!!!\n";
    processTimeOut(currClient);
  }
}

void EventHandler::cgiCondition() {
  ICGI &cgi = *(static_cast<ICGI *>(_currentEvent->udata));
  if (_currentEvent->filter == EVFILT_READ) {
    cgi.waitAndReadCGI();
  } else if (_currentEvent->filter == EVFILT_WRITE) {
    cgi.writeCGI();
  }
}

/**
 * @brief branchCondition : currentEvent의 ident를 기반으로 분기합니다.
 *
 * @details
 * ident가 서버소켓이라면 acceptClient() 함수를 호출합니다.
 * ident가 클라이언트 소켓이라면 clientCondtion() 함수를 호출합니다.
 * ident가 CGI 소켓이라면 cgiCondition() 함수를 호출합니다.
 *
 * @see acceptClient()
 * @see clientCondtion()
 * @see cgiCondition()
 * @see Kqueue::getFdType()
 *
 * @author chanhihi
 * @date 2023.07.17
 */
void EventHandler::branchCondition(void) {
  if (this->_errorFlag == true) return;
  e_fd_type fd_type = Kqueue::getFdType(this->_currentEvent->ident);
  switch (fd_type) {
    case FD_SERVER: {
      acceptClient();
      break;
    }
    case FD_CLIENT: {
      clientCondtion();
      break;
    }
    case FD_CGI:
      cgiCondition();
      break;
    default:
      break;
  }
}

void EventHandler::processRequest(Client &currClient) {
  try {
    if (currClient.getFlag() == RECEIVING) {
      Kqueue::deleteEvent(this->_currentEvent->ident, EVFILT_TIMER,
                          static_cast<void *>(this->_currentEvent->udata));
    }
    Kqueue::addEvent(this->_currentEvent->ident, EVFILT_TIMER,
                     EV_ADD | EV_ONESHOT, NOTE_SECONDS, 60,
                     static_cast<void *>(this->_currentEvent->udata));
    std::cout << "socket descriptor : " << currClient.getSD() << std::endl;
    currClient.receiveRequest();
    currClient.parseRequest(getBoundPort(_currentEvent));
    if (currClient.getFlag() != REQUEST_DONE) return;
    currClient.setFlag(METHOD_SELECT);
    Kqueue::_eventsToAdd.pop_back();
    if (currClient.isCgi()) {
      currClient.makeAndExecuteCgi();
    } else {
      currClient.newHTTPMethod();
      currClient.doRequest();
      currClient.createSuccessResponse();
      Kqueue::disableEvent(currClient.getSD(), EVFILT_READ,
                           static_cast<void *>(&currClient));
      Kqueue::enableEvent(currClient.getSD(), EVFILT_WRITE,
                          static_cast<void *>(&currClient));
    }
  } catch (enum Status &code) {
    if (currClient.getFlag() == RECEIVING) Kqueue::_eventsToAdd.pop_back();
    currClient.createExceptionResponse();
    enableEvent(currClient.getSD(), EVFILT_WRITE,
                static_cast<void *>(&currClient));
  } catch (std::exception &e) {
    disconnectClient(&currClient);
    std::cerr << e.what() << '\n';
    return;
  };
}

void EventHandler::processResponse(Client &currClient) {
  if (currClient.getFlag() != PROCESS_RESPONSE) {
    currClient.setResponseConnection();
    currClient.setFlag(PROCESS_RESPONSE);
  }
  try {
    currClient.sendResponse();
  } catch (std::exception &e) {
    std::cerr << e.what() << '\n';
    disconnectClient(&currClient);
    return;
  };
  if (currClient.getFlag() == END_KEEP_ALIVE) {
    Kqueue::disableEvent(currClient.getSD(), EVFILT_WRITE,
                         static_cast<void *>(&currClient));
    Kqueue::enableEvent(currClient.getSD(), EVFILT_READ,
                        static_cast<void *>(&currClient));
    currClient.clear();
    return;
  }
  if (currClient.getFlag() == END_CLOSE) {
    disconnectClient(&currClient);
    return;
  }
}

void EventHandler::processTimeOut(Client &currClient) {
  currClient.setConnectionClose();
  currClient.createExceptionResponse(E_408_REQUEST_TIMEOUT);
  enableEvent(currClient.getSD(), EVFILT_WRITE,
              static_cast<void *>(&currClient));
}

/**
 * @brief acceptClient : 클라이언트의 연결을 수락합니다.
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
 * @date 2023.07.17
 */
void EventHandler::acceptClient() {
  uintptr_t clientSocket;
  if ((clientSocket = accept(this->_currentEvent->ident, NULL, NULL)) == -1)
    throwWithPerror("accept() error\n" + std::string(strerror(errno)));
#ifdef DEBUG_MSG
  std::cout << "accept : " << clientSocket << std::endl;
#endif
  if (fcntl(clientSocket, F_SETFL, O_NONBLOCK) == -1)
    throwWithPerror("accept fcntl error\n" + std::string(strerror(errno)));
  registClient(clientSocket);
}

/**
 * @brief registClient : 클라이언트 소켓의 이벤트를 등록합니다.
 *
 * @details
 * 클라이언트 소켓의 이벤트는 read, write입니다.
 * EVFILT_READ는 클라이언트로부터 요청(READ EVENT)을 받기 위한 이벤트입니다.
 * EVFILT_WRITE는 클라이언트에게 응답(WRITE EVENT)을 하기 위한 이벤트입니다.
 * READ EVENT는 ENABLE로 등록하여 클라이언트로부터 요청을 받을 수 있도록 합니다.
 * WRITE EVENT는 초기에 DISABLE로 등록하여 클라이언트에게 응답을 하지 않도록
 * 합니다. 클라이언트의 요청을 받은 후에 응답을 하기 위해 ENABLE로 변경합니다.
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
  Kqueue::addEvent(clientSocket, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0,
                   static_cast<void *>(newClient));
  Kqueue::addEvent(clientSocket, EVFILT_WRITE, EV_ADD | EV_DISABLE, 0, 0,
                   static_cast<void *>(newClient));
  Kqueue::setFdSet(clientSocket, FD_CLIENT);
}

void EventHandler::disconnectClient(Client *client) {
  Kqueue::deleteEvent((uintptr_t)client->getSD(), EVFILT_WRITE,
                      static_cast<void *>(client));
  Kqueue::deleteEvent((uintptr_t)client->getSD(), EVFILT_READ,
                      static_cast<void *>(client));
  Kqueue::deleteFdSet((uintptr_t)client->getSD(), FD_CLIENT);

  std::cout << "Client " << client->getSD() << " disconnected!" << std::endl;
  delete client;
}
