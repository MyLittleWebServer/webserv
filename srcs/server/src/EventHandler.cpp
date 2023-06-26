#include "EventHandler.hpp"

EventHandler::EventHandler(const std::vector<Server *> &serverVector) {
  for (std::vector<Server *>::const_iterator it = serverVector.begin();
       it != serverVector.end(); ++it) {
    this->_serverSocketSet.insert((*it)->getSocket());
  }
}

EventHandler::~EventHandler(void) {}

void EventHandler::checkStatus(void) {
  if (_currentEvent->flags & EV_ERROR)
    checkErrorOnSocketVector();
  else
    branchCondition();
}

void EventHandler::checkErrorOnSocketVector() {
  if (_serverSocketSet.find(this->_currentEvent->ident) !=
      this->_serverSocketSet.end())
    throwWithPerror("server socket error");
  else {
    std::cout << " client socket error" << std::endl;
    disconnectClient(_currentEvent->ident, this->_clients);
  }
}

void EventHandler::setCurrentEvent(int i) {
  this->_currentEvent = &(this->_eventList[i]);
}

void EventHandler::branchCondition(void) {
  if (this->_currentEvent->filter == EVFILT_READ) {
    if (_serverSocketSet.find(this->_currentEvent->ident) !=
        this->_serverSocketSet.end())
      acceptClient();
    else if (this->_clients.find(this->_currentEvent->ident) !=
             this->_clients.end()) {
      Client *currClient = this->_clients[this->_currentEvent->ident];
      try {
        std::cout << "socket descriptor : " << currClient->getSD() << std::endl;
        currClient->receiveRequest();
        currClient->newHTTPMethod();
        currClient->getMethod()->parseRequest();
        currClient->getMethod()->matchServerConf(getBoundPort(_currentEvent));
        currClient->getMethod()->validatePath();
        currClient->getMethod()->doRequest();
        currClient->getMethod()->createSuccessResponse();
      } catch (enum Status &code) {
        currClient->getMethod()->createErrorResponse();
      } catch (std::exception &e) {
        disconnectClient(this->_currentEvent->ident, this->_clients);
        std::cerr << e.what() << '\n';
      };
    }
  } else if (this->_currentEvent->filter == EVFILT_WRITE) {
    try {
      this->_clients[this->_currentEvent->ident]->sendResponse(this->_clients);
    } catch (std::exception &e) {
      std::cerr << e.what() << '\n';
    };
    if (this->_clients[this->_currentEvent->ident]->getFlag() == END) {
      disconnectClient(this->_currentEvent->ident, this->_clients);
    }
  }
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
  addEvent(clientSocket, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
  addEvent(clientSocket, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);

  _clients.insert(
      std::pair<int, Client *>(clientSocket, new Client(clientSocket)));
}