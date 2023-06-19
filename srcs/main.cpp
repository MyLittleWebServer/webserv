#include "EventHandler.hpp"
#include "Kqueue.hpp"
#include "Server.hpp"
#include "config/include/Config.hpp"

#ifdef LEAKS
void leakCheck() { system("leaks webserv"); }
#endif

int main(int ac, char** av) {
#ifdef LEAKS
  atexit(leakCheck);
#endif
  if (ac > 2) {
    std::cout << "Usage: ./webserv <config_file(optional)>" << std::endl;
    return (1);
  }
  Config& config = Config::getInstance("config/default.conf");
  (void)config;
  (void)av;
  Server server;
  server.startServer();
  Kqueue eventQueue;

  eventQueue.addEvent(server.getSocket());
  std::cout << "echo server started" << std::endl;

  EventHandler eventHandler(server.getSocket());

  while (1) {
    int eventCount = eventQueue.newEvents();
    for (int i = 0; i < eventCount; ++i) {
      eventHandler.setCurrentEvent(i);
      eventHandler.checkStatus();
    }
  }
  return (0);
}
