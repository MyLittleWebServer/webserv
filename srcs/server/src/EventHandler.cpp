
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

void EventHandler::branchCondition(void) {
  if (this->_errorFlag == true) return;
  if (_serverSocketSet.find(this->_currentEvent->ident) !=
      this->_serverSocketSet.end()) {
    acceptClient();
    return;
  }
  Client &currClient = *(static_cast<Client *>(this->_currentEvent->udata));
  if (this->_currentEvent->filter == EVFILT_READ) {
    if (currClient.getFlag() == RECEIVING) {
      processRequest(currClient);
    } else if (currClient.getFlag() == FILE_IO) {
      // read from file and create response
    } else if (currClient.getFlag() == PIPE_IO) {
      // read from pipe and create response
    }
    return;
  }
  if (this->_currentEvent->filter == EVFILT_WRITE) {
    if (currClient.getFlag() == RESPONSE_CREATED) {
      processResponse(currClient);
    } else if (currClient.getFlag() == FILE_IO) {
      // write to file and create response
    } else if (currClient.getFlag() == PIPE_IO) {
      // write data to pipe for CGI program
    }
    return;
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
      if (currClient.getFdInfo().getFdType() != NONE) {
        registerIOEvent(currClient);
        return;
      }
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
  currClient.setFlag(RESPONSE_CREATED);
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
  registerClient(clientSocket);
}

void EventHandler::registerClient(const uintptr_t clientSocket) {
  Client *newClient = new Client(clientSocket);
  addEvent(clientSocket, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0,
           static_cast<void *>(newClient));
  addEvent(clientSocket, EVFILT_WRITE, EV_ADD | EV_DISABLE, 0, 0,
           static_cast<void *>(newClient));
}

void EventHandler::disconnectClient(const Client *client) {
  Kqueue::deleteEvent((uintptr_t)client->getSD(), EVFILT_WRITE);
  Kqueue::deleteEvent((uintptr_t)client->getSD(), EVFILT_READ);
  close(client->getSD());
  if (client->getMethod() != NULL) delete client->getMethod();
  std::cout << "Client " << client->getSD() << " disconnected!" << std::endl;
  delete client;
}

void EventHandler::registerIOEvent(Client &client) {
  const FdInfo &fdInfo = client.getFdInfo();
  if (fdInfo.getFdType() == FILE_READ) {
    addEvent(fdInfo.getReadFd(), EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0,
             static_cast<void *>(&client));
    return;
  }
  if (fdInfo.getFdType() == FILE_WRITE) {
    addEvent(fdInfo.getWriteFd(), EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0,
             static_cast<void *>(&client));
    return;
  }
  if (fdInfo.getFdType() == PIPE) {
    addEvent(fdInfo.getReadFd(), EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0,
             static_cast<void *>(&client));
    addEvent(fdInfo.getWriteFd(), EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0,
             static_cast<void *>(&client));
    return;
  }
}