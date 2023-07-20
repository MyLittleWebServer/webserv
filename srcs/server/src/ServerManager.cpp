#include "ServerManager.hpp"

#include "Color.hpp"

ServerManager::ServerManager(int ac, char **av) {
  if (ac > 2 || ac < 1)
    throwWithErrorMessage("Usage: ./webserv [config_file]");
  else if (ac == 2)
    Config::getInstance(av[1]);
  else
    Config::getInstance("config/default.conf");
}

void ServerManager::initConfig(void) {
  try {
    Config &config = Config::getInstance();
    std::list<IServerConfig *> serverInfo = config.getServerConfigs();
    std::list<IServerConfig *>::iterator it = serverInfo.begin();
    while (it != serverInfo.end()) {
      _listenOrganizer.insert((*it)->getListen());
      ++it;
    }
    std::cout << "Config initialized" << std::endl;
  } catch (std::exception &e) {
    std::cout << e.what() << std::endl;
  }
}

void ServerManager::initServer(void) {
  std::set<short>::iterator it = _listenOrganizer.begin();
  try {
    while (it != _listenOrganizer.end()) {
      Server *server = new Server(*it);
      server->initServerSocket();
      std::cout << "server: " << server->getPort();
      _eventQueue.addEvent(server->getSocket());
      Kqueue::setFdSet(server->getSocket(), FD_SERVER);
      _serverVector.push_back(server);
      std::cout << ":" << server->getSocket() << std::endl;
      ++it;
    }
    std::cout << "Server initialized" << std::endl;
  } catch (std::exception &e) {
    std::cout << e.what() << std::endl;
  }
}

void ServerManager::startServer(void) {
  try {
    std::cout << "Server started" << std::endl;
    EventHandler eventHandler(_serverVector);
    while (1) {
      int eventCount = _eventQueue.newEvents();
      for (int i = 0; i < eventCount; ++i) {
        eventHandler.setCurrentEvent(i);
        eventHandler.checkFlags();
        eventHandler.branchCondition();
      }
    }
  } catch (std::exception &e) {
    std::cout << e.what() << std::endl;
  }
}

void ServerManager::promptServer(void) {
  std::cout << "\n-------- [ " BOLDBLUE << "Web Server Info" << RESET
            << " ] --------\n"
            << std::endl;

  std::vector<Server *>::const_iterator it = this->_serverVector.begin();
  for (; it != this->_serverVector.end(); ++it) {
    std::cout << "socket: " << BOLDGREEN << (*it)->getSocket() << RESET;
    std::cout << "   | host: " << BOLDGREEN << (*it)->getHost() << RESET;
    std::cout << "   | port: " << BOLDGREEN << (*it)->getPort() << RESET
              << std::endl
              << std::endl;
  }
  std::cout << "------------------------------------" << std::endl;
}
