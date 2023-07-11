
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
    return;
  }
  processResponse(currClient);
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
      break;
    default:
      break;
  }
}

void EventHandler::processRequest(Client &currClient) {
  try {
    std::cout << "socket descriptor : " << currClient.getSD() << std::endl;
    currClient.receiveRequest();
    currClient.parseRequest(getBoundPort(_currentEvent));
    if (currClient.getFlag() == RECEIVING) return;
    currClient.newHTTPMethod();
    if (currClient.isCgi())
      (void)NULL;  // cgi 처리 로직
    else {
      currClient.doRequest();
    }
    currClient.createSuccessResponse();
    enableEvent(currClient.getSD(), EVFILT_WRITE,
                static_cast<void *>(&currClient));
  } catch (enum Status &code) {
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
  };
  disableEvent(currClient.getSD(), EVFILT_WRITE,
               static_cast<void *>(&currClient));
  if (currClient.getFlag() == END) {
    disconnectClient(&currClient);
    return;
  }
  // TODO : init member
  currClient.setFlag(RECEIVING);
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

void EventHandler::disconnectClient(const Client *client) {
  Kqueue::deleteEvent((uintptr_t)client->getSD(), EVFILT_WRITE);
  Kqueue::deleteEvent((uintptr_t)client->getSD(), EVFILT_READ);
  Kqueue::deleteFdSet((uintptr_t)client->getSD(), FD_CLIENT);
  close(client->getSD());
  if (client->getMethod() != NULL) delete client->getMethod();
  std::cout << "Client " << client->getSD() << " disconnected!" << std::endl;
  delete client;
}