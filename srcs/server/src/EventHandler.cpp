#include "EventHandler.hpp"

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
    processRequest(currClient);
    return;
  }
  processResponse(currClient);
}

void EventHandler::processRequest(Client &currClient) {
  try {
    std::cout << "socket descriptor : " << currClient.getSD() << std::endl;
    currClient.receiveRequest();
    currClient.newHTTPMethod();
    currClient.getMethod()->parseRequest();
    currClient.getMethod()->matchServerConf(getBoundPort(_currentEvent));
    currClient.getMethod()->validatePath();
    currClient.getMethod()->doRequest();
    currClient.getMethod()->createSuccessResponse();
  } catch (enum Status &code) {
    currClient.getMethod()->createErrorResponse();
  } catch (std::exception &e) {
    disconnectClient(&currClient);
    std::cerr << e.what() << '\n';
  };
}

void EventHandler::processResponse(Client &currClient) {
  try {
    currClient.sendResponse();
  } catch (std::exception &e) {
    std::cerr << e.what() << '\n';
  };
  if (currClient.getFlag() == END) {
    disconnectClient(&currClient);
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
  Client *newClient = new Client(clientSocket);
  addEvent(clientSocket, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0,
           static_cast<void *>(newClient));
  addEvent(clientSocket, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0,
           static_cast<void *>(newClient));
}