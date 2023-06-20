#include "Utils.hpp"

void exitWithPerror(const std::string &msg) {
  std::cerr << msg << std::endl;
  exit(EXIT_FAILURE);
}

void disconnectClient(int client_fd, std::map<int, Client> &clients) {
  close(client_fd);
  clients.erase(client_fd);
  std::cout << "Client " << client_fd << " disconnected!" << std::endl;
}
