
#include "EventHandler.hpp"

#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

EventHandler::EventHandler(const std::vector<Server *> &serverVector)
    : _errorFlag(false) {
  for (std::vector<Server *>::const_iterator it = serverVector.begin();
       it != serverVector.end(); ++it) {
    this->_serverSocketSet.insert((*it)->getSocket());
    delete (*it);
  }
}

EventHandler::~EventHandler(void) {}

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

void EventHandler::setCurrentEvent(int i) {
  this->_currentEvent = &(this->_eventList[i]);
}

void EventHandler::clientCondtion() {
  Client &currClient = *(static_cast<Client *>(this->_currentEvent->udata));
  if (this->_currentEvent->filter == EVFILT_READ) {
    processRequest(currClient);
  } else if (this->_currentEvent->filter == EVFILT_WRITE) {
    processResponse(currClient);
  } else if (this->_currentEvent->filter == EVFILT_TIMER) {
    // create timeout response
    std::cout << "TIMEOUT OCCORRED!!!!\n";
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
    currClient.createErrorResponse();
    enableEvent(currClient.getSD(), EVFILT_WRITE,
                static_cast<void *>(&currClient));
  } catch (std::exception &e) {
    disconnectClient(&currClient);
    std::cerr << e.what() << '\n';
    return;
  };
}

void EventHandler::processResponse(Client &currClient) {
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
  currClient.createErrorResponse(E_408_REQUEST_TIMEOUT);
  enableEvent(currClient.getSD(), EVFILT_WRITE,
              static_cast<void *>(&currClient));
}

void EventHandler::acceptClient() {
  uintptr_t clientSocket;
  if ((clientSocket = accept(this->_currentEvent->ident, NULL, NULL)) == -1)
    throwWithPerror("accept() error\n" + std::string(strerror(errno)));
  std::cout << "accept ... : " << clientSocket << std::endl;
  fcntl(clientSocket, F_SETFL, O_NONBLOCK);
  registClient(clientSocket);
}

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
