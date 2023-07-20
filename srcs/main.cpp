#include "ServerManager.hpp"

#ifdef LEAKS
void leakCheck() { system("leaks webserv"); }
#endif

int main(int ac, char **av) {
#ifdef LEAKS
  atexit(leakCheck);
#endif
  try {
    signal(SIGPIPE, SIG_IGN);
    Kqueue::init();
    ServerManager serverManager(ac, av);
    serverManager.initConfig();
    serverManager.initServer();
    serverManager.promptServer();
    serverManager.startServer();
  } catch (...) {
    return (errno);
  }
}
