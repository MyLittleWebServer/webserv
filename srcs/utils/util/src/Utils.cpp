#include "Utils.hpp"

#include <ctime>

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

std::string getCurrentTime() {
  std::time_t t = std::time(NULL);  // 현재 시간을 얻습니다.
  std::tm *timePtr = std::gmtime(&t);  // 현재 시간을 UTC/GMT로 변환합니다.

  char buffer[1000];
  strftime(buffer, sizeof(buffer), "Date: %a, %d %b %Y %H:%M:%S GMT",
           timePtr);  // 시간을 원하는 형식으로 포맷팅합니다.

  std::string date(buffer);  // 변환된 문자열을 std::string에 저장합니다.

  return (date);
}

std::string itos(int num) {
  std::stringstream ss;
  ss << num;
  return (ss.str());
}