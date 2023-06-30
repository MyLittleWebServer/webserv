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

void disconnectClient(const Client *client) {
  Kqueue::deleteEvent((uintptr_t)client->getSD(), EVFILT_WRITE);
  Kqueue::deleteEvent((uintptr_t)client->getSD(), EVFILT_READ);
  close(client->getSD());
  if (client->getMethod() != NULL) delete client->getMethod();
  std::cout << "Client " << client->getSD() << " disconnected!" << std::endl;
  delete client;
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

std::string toLowerString(std::string str) {
  std::string::iterator it = str.begin();
  std::string::iterator end = str.end();

  while (it != end) {
    *it = std::tolower(*it);
    ++it;
  }
  return str;
}
