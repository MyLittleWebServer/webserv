#ifndef EventHandler_HPP
#define EventHandler_HPP

#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/event.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include <iostream>
#include <map>
#include <queue>
#include <set>
#include <vector>

#include "Client.hpp"
#include "ExceptionThrower.hpp"
#include "Kqueue.hpp"
#include "Server.hpp"
#include "Utils.hpp"

class Client;

class EventHandler : public Kqueue {
 private:
  std::set<uintptr_t> _serverSocketSet;
  struct kevent* _currentEvent;
  bool _errorFlag;

  void checkErrorOnSocket(void);
  void acceptClient(void);
  void registClient(const uintptr_t clientSocket);
  void processRequest(Client& client);
  void processResponse(Client& client);

 public:
  EventHandler(const std::vector<Server*>& serverVector);
  virtual ~EventHandler();

  void setCurrentEvent(int i);
  void checkFlags(void);
  void branchCondition(void);
};

#endif