/**
 * @file ServerManager.cpp
 * @brief 서버들을 관리하는 소스파일입니다.
 * @details
 * ServerManager는 다음과 같은 역할을 합니다.
 * 1. Config를 초기화합니다.
 * 2. Server를 초기화합니다.
 * 3. Server의 정보를 출력합니다.
 * 4. Server를 시작합니다.
 *
 * @author TocaTocaToca
 * @date 2023-07-20
 * @copyright Copyright (c) 2023
 */

#include "ServerManager.hpp"

#include "Color.hpp"
#include "Session.hpp"

/**
 * @brief ServerManager 함수는 ServerManager를 생성합니다.
 *
 * @details
 * webserv 프로그램을 시작합니다.
 * webserv를 실행할 때 ./webserv config 파일이 들어있으면 해당 config파일에
 * 맞춰서 getInstance를 호출합니다. 만약 ./webserv로 그냥 실행 한다면
 * config/default.conf를 인자로 getInstance를 호출합니다.
 *
 * @exception Usage: ./webserv [config_file] : 인자가 2개 이상이거나 0개 이하
 *
 * @param ac number of arguments
 * @param av arguments
 *
 * @author chanhihi
 * @date 2023.07.17
 */
ServerManager::ServerManager(int ac, char **av) {
  if (ac > 2 || ac < 1)
    throwWithErrorMessage("Usage: ./webserv [config_file]");
  else if (ac == 2)
    Config::getInstance(av[1]);
  else
    Config::getInstance("config/default.conf");
}

/**
 * @brief initSignal 함수는 signal를 초기화합니다.
 *
 * @details
 * signal(SIGPIPE, SIG_DFL) 의 경우 소켓 및 파이프가 끊겼을 경우 프로세스가 죽는
 * 현상이 발생합니다. Server manager에서 ignore을 한번 선언하여 SIGPIPE를
 * 무시하고 자체적으로 처리합니다.
 *
 * @author chanhihi
 * @date 2023.07.17
 */
void ServerManager::initSignal(void) { signal(SIGPIPE, SIG_IGN); }

/**
 * @brief initServer 함수는 Config에 저장된 포트를 기반으로 서버를
 * 초기화합니다.
 *
 * @details
 * 서버를 초기화하는 과정은 다음과 같습니다.
 * 1. 동일한 포트를 중복으로 바인딩하는 것을 막기 위해 set에 바인딩할 포트들을
 *    삽입합니다.
 * 2. Server 객체를 생성합니다.
 * 3. Server 객체의 initServerSocket() 함수를 호출합니다.
 * 4. Server 객체의 getSocket() 함수를 호출하여 반환된 소켓 디스크립터를
 *    이벤트 큐 추가 대기열에 추가합니다.
 * 5. Server 객체의 getSocket() 함수를 호출하여 반환된 소켓 디스크립터를
 *    Kqueue 클래스의 setFdSet() 함수를 호출하여 FD_SERVER 타입으로
 *    설정합니다.
 *
 * @see Server
 * @see Kqueue
 * @see EventHandler
 *
 * @exception std::exception
 * 서버를 초기화하는 과정에서 예외가 발생하면 예외를 던집니다.
 *
 * @author chanhihi
 * @date 2023.07.17
 */
void ServerManager::initServer(void) {
  Kqueue::init();
  Config &config = Config::getInstance();
  std::list<IServerConfig *> serverInfo = config.getServerConfigs();
  std::list<IServerConfig *>::iterator it = serverInfo.begin();
  std::vector<Server> serverVector;
  try {
    std::set<short> ports;
    for (; it != serverInfo.end(); ++it) {
      ports.insert((*it)->getListen());
    }
    std::set<short>::iterator it = ports.begin();
    for (; it != ports.end(); ++it) {
      serverVector.push_back(Server(*it));
      serverVector.back().initServerSocket();
      _eventQueue.addEvent(serverVector.back().getSocket());
      Kqueue::setFdSet(serverVector.back().getSocket(), FD_SERVER);
    }
  } catch (std::exception &e) {
    std::cout << e.what() << std::endl;
  }
  promptServer(serverVector);
}

/**
 * @brief promptServer 함수는 서버의 정보를 출력합니다.
 *
 * @details
 * initServer() 함수에서 생성된 serverVector에 들어있는 서버의 정보를
 * 출력합니다.
 *
 * @see initServer
 * @see Server
 * @see getSocket
 * @see getHost
 * @see getPort
 *
 * @author chanhihi
 * @date 2023.07.17
 */
void ServerManager::promptServer(std::vector<Server> &serverVector) {
  std::cout << "\n-------- [ " BOLDBLUE << "Web Server Info" << RESET
            << " ] --------\n"
            << std::endl;

  std::vector<Server>::const_iterator it = serverVector.begin();
  for (; it != serverVector.end(); ++it) {
    std::cout << "socket: " << BOLDGREEN << (*it).getSocket() << RESET;
    std::cout << "   | host: " << BOLDGREEN << (*it).getHost() << RESET;
    std::cout << "   | port: " << BOLDGREEN << (*it).getPort() << RESET
              << std::endl
              << std::endl;
  }
  std::cout << "------------------------------------" << std::endl;
}

/**
 * @brief startServer 함수는 kqueue에 등록된 이벤트를 기반으로 서버를
 * 가동합니다.
 *
 * @details
 * EventHandler 객체를 생성합니다.
 * EventHandler 객체의 setCurrentEvent() 함수를 호출하여 현재 이벤트를
 * 설정합니다. EventHandler 객체의 checkFlags() 함수를 호출하여 현재 이벤트의
 * 플래그를 확인합니다. EventHandler 객체의 branchCondition() 함수를 호출하여
 * 현재 이벤트의 플래그에 따라 분기합니다.
 *
 * @see EventHandler
 * @see setCurrentEvent
 * @see checkFlags
 * @see branchCondition
 *
 * @exception std::exception
 * 서버가 시작되어 작동하는 과정에서 예외가 발생하면 예외를 던집니다.
 *
 * @author chanhihi
 * @date 2023.07.17
 */
void ServerManager::startServer(void) {
  try {
    std::cout << "Server started" << std::endl;
    EventHandler &eventHandler = EventHandler::getInstance();
    _eventQueue.addSessionTimerEvent();
    while (1) {
      int eventCount = _eventQueue.newEvents();
      for (int i = 0; i < eventCount; ++i) {
        eventHandler.setCurrentEvent(i);
        eventHandler.checkFlags();
        eventHandler.branchCondition();
      }
    }
  } catch (std::exception &e) {
    std::cout << e.what() << std::endl;
  }
}
