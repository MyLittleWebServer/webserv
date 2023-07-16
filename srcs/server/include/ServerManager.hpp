#pragma once

#include <set>

#include "Config.hpp"
#include "EventHandler.hpp"
#include "Server.hpp"

/**
 * @brief ServerManager class
 *
 * ServerManager는 서버를 관리하는 클래스이다.
 *
 * @details
 * Kqueue 객체 _eventQueue를 갖고있습니다.
 * ServerManager는 Config를 초기화하고, Server를 초기화합니다.
 * 그리고 Server를 시작하고, Server 시작을 prompt에 알립니다.
 *
 * @see Kqueue
 * @see initConfig
 * @see initServer
 * @see startServer
 * @see promptServer
 *
 * @note
 * ServerManager는 Server를 관리하는 클래스이다.
 *
 * @author chanhihi
 * @date 2021-07-05
 */
class ServerManager {
 private:
  std::vector<Server *> _serverVector;
  std::set<short> _listenOrganizer;
  Kqueue _eventQueue;

 public:
  ServerManager(int ac, char **av);
  void initConfig(void);
  void initServer(void);
  void startServer(void);
  void promptServer(void);
};
