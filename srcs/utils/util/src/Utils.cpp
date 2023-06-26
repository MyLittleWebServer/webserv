#include "Utils.hpp"

#include <netinet/in.h>
#include <sys/event.h>
#include <sys/socket.h>
#include <sys/stat.h>

#include <ctime>

#include "Kqueue.hpp"

void throwWithPerror(const std::string &msg) {
  std::cerr << msg << std::endl;
  throw(EXIT_FAILURE);
}

void disconnectClient(int client_fd, std::map<int, Client *> &clients) {
  std::map<int, Client *>::iterator it = clients.find(client_fd);
  if (it == clients.end()) {
    std::cerr << "Client " << client_fd << " not found!" << std::endl;
    return;
  }
  Kqueue::deleteEvent((uintptr_t)client_fd, EVFILT_WRITE);
  Kqueue::deleteEvent((uintptr_t)client_fd, EVFILT_READ);
  close(client_fd);
  if (clients[client_fd]->_method != NULL) {
    delete clients[client_fd]->getMethod();
    clients[client_fd]->_method = NULL;
  }
  delete clients[client_fd];
  clients.erase(client_fd);
  std::cout << "Client " << client_fd << "disconnected!" << std::endl;
}

short getBoundPort(const struct kevent *_currentEvent) {
  struct sockaddr_in addr;
  socklen_t addr_len = sizeof(addr);
  if (getsockname(_currentEvent->ident, (struct sockaddr *)&addr, &addr_len) ==
      -1)
    throw std::runtime_error("getsockname() error\n");
  std::cout << "getsockname: " << ntohs(addr.sin_port) << std::endl;
  return (ntohs(addr.sin_port));
}

std::string getCurrentTime() {
  std::time_t t = std::time(NULL);
  std::tm *timePtr = std::gmtime(&t);

  char buffer[1000];
  std::strftime(buffer, sizeof(buffer), "Date: %a, %d %b %Y %H:%M:%S GMT",
                timePtr);

  std::string date(buffer);

  return (date);
}

std::string itos(int num) {
  std::stringstream ss;
  ss << num;
  return (ss.str());
}
