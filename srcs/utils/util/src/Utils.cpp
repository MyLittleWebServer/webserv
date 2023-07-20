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

std::string ft_trimOWS(std::string &str) {
  std::string result = str;
  size_t pos = result.find_first_not_of(" \t");
  if (pos != std::string::npos) {
    result.erase(0, pos);
  }
  pos = result.find_last_not_of(" \t");
  if (pos != std::string::npos) {
    result.erase(pos + 1);
  }
  return result;
}

std::string ft_trim(std::string &str) {
  if (str.empty()) return str;
  std::string result = str;
  size_t pos = 0;
  while (pos < result.size() && std::isspace(result[pos])) pos++;
  result.erase(0, pos);
  pos = result.size() - 1;
  while (pos > 0 && std::isspace(result[pos])) pos--;
  result.erase(pos + 1);
  return result;
}

std::vector<std::string> ft_split(const std::string &str, char delim) {
  std::vector<std::string> result;
  std::istringstream iss(str);
  std::string token;

  while (std::getline(iss, token, delim)) {
    result.push_back(token);
  }
  return result;
}