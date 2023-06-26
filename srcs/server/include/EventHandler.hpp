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
  std::map<int, Client*> _clients;
  std::set<uintptr_t> _serverSocketSet;
  struct kevent* _currentEvent;

  void registClient(const uintptr_t clientSocket);
  void acceptClient(void);
  void branchCondition(void);

 public:
  EventHandler(const std::vector<Server*>& serverVector);
  virtual ~EventHandler();

  void setCurrentEvent(int i);
  void checkStatus(void);
  void checkErrorOnSocketVector(void);
};

#endif