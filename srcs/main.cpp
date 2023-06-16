#include "config/include/Config.hpp"

int main() {
  Config& config = Config::getInstance("config/default.conf");
  (void)config;
  return 0;
}
