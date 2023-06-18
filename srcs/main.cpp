#include "config/include/Config.hpp"

void leakCheck() { system("leaks webserv"); }

int main() {
  atexit(leakCheck);
  Config& config = Config::getInstance("config/default.conf");
  (void)config;
  return 0;
}
