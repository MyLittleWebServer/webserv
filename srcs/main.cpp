#include "config/include/Config.hpp"

int main() {
  Config& config = Config::getInstance("config/mime.types");
  std::cout << config.getUser() << std::endl;
  std::cout << config.getPort() << std::endl;
  return 0;
}
