#include "Utils.hpp"

void throwWithPerror(const std::string &msg) {
  std::cerr << msg << std::endl;
  throw(EXIT_FAILURE);
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
  std::strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S GMT", timePtr);

  return (std::string(buffer));
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
