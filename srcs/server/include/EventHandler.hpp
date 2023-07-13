#ifndef EventHandler_HPP
#define EventHandler_HPP

#include <sys/event.h>

#include <set>

#include "Client.hpp"
#include "Kqueue.hpp"
#include "Server.hpp"

class Client;

class EventHandler : public Kqueue {
 private:
  std::set<uintptr_t> _serverSocketSet;
  struct kevent* _currentEvent;
  bool _errorFlag;

  void checkErrorOnSocket(void);
  void acceptClient(void);
  void disconnectClient(Client* client);
  void registClient(const uintptr_t clientSocket);
  void processRequest(Client& client);
  void processResponse(Client& client);
  void processTimeOut(Client& client);

  void clientCondtion();
  void cgiCondition();

 public:
  EventHandler(const std::vector<Server*>& serverVector);
  virtual ~EventHandler();

  void setCurrentEvent(int i);
  void checkFlags(void);
  void branchCondition(void);
};

#endif
