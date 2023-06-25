#include <set>
#include <vector>

#include "Config.hpp"
#include "EventHandler.hpp"
#include "Kqueue.hpp"
#include "Server.hpp"

#ifdef LEAKS
void leakCheck() { system("leaks webserv"); }
#endif

int main(int ac, char **av) {
#ifdef LEAKS
  atexit(leakCheck);
#endif
  try {
    mainValidator(ac, av);
    Kqueue eventQueue;

    std::vector<Server *> serverVector;
    std::set<size_t> listenOrganizer;

    // Config &config = Config::getInstance();
    // std::list<IServerConfig *> serverInfo = config.getServerConfigs();
    // std::list<IServerConfig *>::iterator it = serverInfo.begin();
    // while (it != serverInfo.end()) {
    //   std::cout << "set : " << (*it)->getListen() << std::endl;
    //   listenOrganizer.insert((*it)->getListen());
    //   ++it;
    // }

    // listenOrganizer.insert(8080);
    // listenOrganizer.insert(8080);
    listenOrganizer.insert(8081);
    listenOrganizer.insert(8082);
    listenOrganizer.insert(8083);

    std::set<size_t>::iterator it2 = listenOrganizer.begin();

    while (it2 != listenOrganizer.end()) {
      std::cout << "server : " << *it2 << std::endl;
      Server *server = new Server(*it2);
      serverVector.push_back(server);
      serverVector.back()->startServer();
      std::cout << "server : " << server->getSocket() << std::endl;
      eventQueue.addEvent(server->getSocket());
      ++it2;
    }

    EventHandler eventHandler(serverVector);

    std::cout << "echo server started" << std::endl;

    while (1) {
      int eventCount = eventQueue.newEvents();
      for (int i = 0; i < eventCount; ++i) {
        eventHandler.setCurrentEvent(i);
        eventHandler.checkStatus();
      }
    }
  } catch (std::exception &e) {
    std::cout << e.what() << std::endl;
  } catch (...) {
    std::cout << "fatal error" << std::endl;
  }
}
