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
  std::vector<Server*> _serverVector;
  struct kevent* _currentEvent;

  void registClient(const uintptr_t clientSocket);
  void acceptClient(uintptr_t serverSocket);
  void branchCondition(uintptr_t serverSocket);

 public:
  EventHandler(std::vector<Server*> serverQueue);
  virtual ~EventHandler();

  void setCurrentEvent(int i);
  void checkStatus();
  void checkErrorOnSocketVector(uintptr_t serverSocket);
};

#endif