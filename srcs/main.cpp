/**
 * @file main.cpp
 * @brief 프로그램의 시작점입니다.
 * @details
 * @author chanhihi
 * @date 2023-07-20
 * @copyright Copyright (c) 2023
 */

/**
   @mainpage   Webserv
   @section intro Introduction
    - C++ 98 기준으로 작성한 웹서버입니다.
    - RFC 7230, 7231, 7232, 7233, 7234, 7235를 준수합니다.
    - HTTP/1.1을 지원합니다.
    - GET, POST, PUT, DELETE Method를 지원합니다.
    - CGI 를 지원합니다.
    - 다중 Listen(Config)를 지원합니다.
    - 다중 Server(Config)를 지원합니다.

   @section Program Webserv
   - Config : 설정 파일 내용을 관리하는 정보성 클래스
   - Server : 서버를 정보를 담고있는 클래스
   - Client : 클라이언트를 정보를 담당하는 클래스
   - Request : 클라이언트의 요청을 담당하는 클래스
   - Response : 서버의 응답을 담당하는 클래스


   @section  INOUTPUT    Web Server Info
   -------- [ Web Server Info ] --------

  socket: 4   | host: chanheki  | port: 3000

  socket: 5   | host: jincpark   | port: 3000

  socket: 6   | host: sechung   | port: 3030

  socket: 7   | host: jang-cho   | port: 3030

    -------- [ TOCA TOCA TOCA ] --------

   @section  CREATEINFO      Docs Created Info
   - @author chanhihi
   - @date 2023-7-17
   @section  MODIFYINFO      Docs Modified Info
   - 수정자/수정일   : 수정내역
   - chanhihi/2023-7-17 : 최초작성
   */

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
    ServerManager serverManager(ac, av);
    serverManager.initSignal();
    serverManager.initServer();
    serverManager.startServer();
  } catch (...) {
    return (errno);
  }
  return 0;
}
