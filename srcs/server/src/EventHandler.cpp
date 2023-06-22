#include "EventHandler.hpp"

EventHandler::EventHandler(uintptr_t serverSocket)
    : _serverSocket(serverSocket) {}

EventHandler::~EventHandler(void) {}

void EventHandler::checkStatus(void) {
  if (_currentEvent->flags & EV_ERROR)
    checkErrorOnSocket();
  else
    branchCondition();
}

void EventHandler::checkErrorOnSocket() {
  if (_currentEvent->ident == _serverSocket)
    exitWithPerror("server socket error");
  else {
    std::cerr << "client socket error" << std::endl;
    disconnectClient(_currentEvent->ident, _clients);
  }
}

void EventHandler::setCurrentEvent(int i) {
  this->_currentEvent = &(this->_eventList[i]);
}

void EventHandler::branchCondition() {
  if (this->_currentEvent->filter == EVFILT_READ) {
    if (this->_currentEvent->ident == _serverSocket)
      acceptClient();
    else if (this->_clients.find(this->_currentEvent->ident) !=
             this->_clients.end()) {
      try {
        Client &currClient = this->_clients[this->_currentEvent->ident];
        currClient.receiveRequest();
        currClient.newHTTPMethod();
        currClient.getMethod()->parseRequest();
        currClient.getMethod()->matchServerConf(getBoundPort(_currentEvent));
        currClient.getMethod()->validatePath();
        currClient.getMethod()->doRequest();
        currClient.getMethod()->createResponse();
      } catch (std::exception &e) {
        disconnectClient(this->_currentEvent->ident, this->_clients);
        std::cerr << e.what() << '\n';
      };
    }
  } else if (this->_currentEvent->filter == EVFILT_WRITE) {
    try {
      this->_clients[this->_currentEvent->ident].sendResponse(this->_clients);
    } catch (std::exception &e) {
      std::cerr << e.what() << '\n';
    };
  }
}

void EventHandler::acceptClient() {
  uintptr_t clientSocket;
  if ((clientSocket = accept(_serverSocket, NULL, NULL)) == -1)
    exitWithPerror("accept() error\n" + std::string(strerror(errno)));
  std::cout << "accept new client: " << clientSocket << std::endl;
  fcntl(clientSocket, F_SETFL, O_NONBLOCK);

  registClient(clientSocket);
}

void EventHandler::registClient(const uintptr_t clientSocket) {
  addEvent(clientSocket, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
  addEvent(clientSocket, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);

  _clients.insert(std::pair<int, Client>(clientSocket, Client(clientSocket)));
}
