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
#include <vector>

#include "Client.hpp"
#include "Kqueue.hpp"
#include "Utils.hpp"

class Client;

class EventHandler : public Kqueue {
 private:
  std::map<int, Client> _clients;
  const uintptr_t _serverSocket;
  struct kevent* _currentEvent;

  void acceptClient();
  void registClient(const uintptr_t clientSocket);

 public:
  EventHandler(uintptr_t serverSocket);
  EventHandler(const EventHandler& src);
  virtual ~EventHandler();
  EventHandler& operator=(EventHandler const& rhs);

  void checkStatus();
  void checkErrorOnSocket();
  void setCurrentEvent(int i);
  void branchCondition();
};

#endif