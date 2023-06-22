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

short getBoundPort(const struct kevent *_currentEvent) {
  struct sockaddr_in addr;
  socklen_t addr_len = sizeof(addr);
  if (getsockname(_currentEvent->ident, (struct sockaddr *)&addr, &addr_len) ==
      -1)
    throw std::runtime_error("getsockname() error\n");
  return (ntohs(addr.sin_port));
}
