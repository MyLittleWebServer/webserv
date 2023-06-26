#pragma once

#include <set>

#include "Config.hpp"
#include "EventHandler.hpp"
#include "Server.hpp"

class ServerManager {
 private:
  std::vector<Server *> _serverVector;
  std::set<short> _listenOrganizer;
  Kqueue _eventQueue;

 public:
  ServerManager(int ac, char **av);
  void initConfig(void);
  void initServer(void);
  void startServer(void);
  void promptServer(void);
};
