#include "ServerManager.hpp"

#ifdef LEAKS
void leakCheck() { system("leaks webserv"); }
#endif

int main(int ac, char **av) {
#ifdef LEAKS
  atexit(leakCheck);
#endif
  try {
    ServerManager serverManager(ac, av);
    serverManager.initConfig();
    serverManager.initServer();
    serverManager.promptServer();
    serverManager.startServer();
  } catch (...) {
    std::cout << "fatal error" << std::endl;
  }
}
