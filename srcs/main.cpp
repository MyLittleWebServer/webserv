#include "ServerManager.hpp"

#ifdef LEAKS
void leakCheck() { system("leaks webserv"); }
#endif

/**
 * @brief 프로그램 시작
 *
 * @details
 * kqueue를 초기화하고 ServerManager를 생성하여 서버를 시작합니다.
 * ServerManager는 Config를 초기화하고, Server를 초기화합니다.
 * 그리고 Server를 시작하고, Server 시작을 prompt에 알립니다.
 *
 * @see Kqueue
 * @see ServerManager
 *
 * @param ac number of arguments
 * @param av arguments
 * @return int
 *
 * @author chanhihi
 * @date 2023.07.17
 */
int main(int ac, char **av) {
#ifdef LEAKS
  atexit(leakCheck);
#endif
  try {
    Kqueue::init();
    ServerManager serverManager(ac, av);
    serverManager.initConfig();
    serverManager.initServer();
    serverManager.promptServer();
    serverManager.startServer();
  } catch (...) {
    std::cout << "Fatal Error" << std::endl;
  }
  return 0;
}
