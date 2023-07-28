#ifndef Client_HPP
#define Client_HPP

#include <fcntl.h>
#include <sys/socket.h>
#include <unistd.h>

#include <iostream>
#include <map>
#include <string>

#include "CGI.hpp"
#include "ICGI.hpp"
#include "IMethod.hpp"
#include "Request.hpp"
#include "Response.hpp"

#define RECEIVE_LEN 1460

/**
 * @brief 클라이언트의 상태를 나타내는 enum입니다.
 *
 * @details
 * 1. START : 클라이언트가 생성되었을 때의 상태입니다.
 * 2. RECEIVING : 클라이언트가 요청을 수신중일 때의 상태입니다.
 * 3. RECEIVE_DONE : 클라이언트가 요청을 완전히 수신했을 때의 상태입니다.
 * 4. REQUEST_DONE : 클라이언트가 요청을 완전히 파싱했을 때의 상태입니다.
 * 5. PROCESS_RESPONSE : 클라이언트가 응답 생성 및 전송중일 때의 상태입니다.
 * 6. END_KEEP_ALIVE : 클라이언트가 Keep-Alive를 종료할 때의 상태입니다.
 * 7. END_CLOSE : 클라이언트가 연결을 종료할 때의 상태입니다.
 *
 * @see Client
 * @see EventHandler
 * @see IMethod
 * @see ICGI
 *
 * @author chanhihi
 * @date 2021-07-21
 */
enum ClientStates {
  START,
  RECEIVING,
  EXPECT_CONTINUE,
  METHOD_SELECT,
  EXPECT_CONTINUE_PROCESS_RESPONSE,
  PROCESS_RESPONSE,
  RESPONSE_DONE,
  END_KEEP_ALIVE,
  END_CLOSE
};

class Utils;

/**
 * @brief 클라이언트를 관리하는 클래스입니다.
 *
 * @details
 * 1. 클라이언트의 상태를 나타내는 enum ClientStates를 가집니다.
 *
 */
class Client {
 private:
  ClientStates _state;
  uintptr_t _sd;
  std::string _recvBuff;
  Request _request;
  Response _response;
  IMethod *_method;
  ICGI *_cgi;
  size_t _lastSentPos;

  int _keepAliveTimeOutLimit;
  int _keepAliveTimeOutUnit;
  int _requestTimeOutLimit;
  int _requestTimeOutUnit;

  bool _sessionConfig;

  static char _buf[RECEIVE_LEN];

  bool checkIfReceiveFinished(ssize_t n);
  // std::map<uintptr_t, char *> _clientBuf;

 private:
  void contentNegotiation();
  void headMethodBodyCheck();
  void setResponseConnection();
  void reassembleResponse();
  void methodNotAllowCheck();

 public:
  Client();
  Client(const uintptr_t sd);
  Client &operator=(const Client &src);
  virtual ~Client();

 public:
  uintptr_t getSD() const;
  IMethod *getMethod() const;
  void receiveRequest();
  void newHTTPMethod();
  void sendResponse();
  void setState(ClientStates state);
  void parseRequest(short port);
  bool isCgi();
  void doRequest();
  void createExceptionResponse();
  void createExceptionResponse(Status statusCode);
  void createSuccessResponse();
  void makeAndExecuteCgi();
  void clear();
  void setConnectionClose();
  void createContinueResponse();
  void responseFinalCheck();

  ClientStates getState() const;

  void initTimeOut(short serverPort);
  void initTimeOutLimitAndUnit(const std::string &str, int &limit, int &unit);
  int getKeepAliveTimeOutLimit() const;
  int getKeepAliveTimeOutUnit() const;
  int getRequestTimeOutLimit() const;
  int getRequestTimeOutUnit() const;

  void initSessionConfig(short serverPort);
  bool getSessionConfig() const;

  class RecvFailException : public std::exception {
   public:
    const char *what() const throw();
  };
  class DisconnectedDuringRecvException : public std::exception {
   public:
    const char *what() const throw();
  };

  class SendFailException : public std::exception {
   public:
    const char *what() const throw();
  };
  class DisconnectedDuringSendException : public std::exception {
   public:
    const char *what() const throw();
  };
};

std::ostream &operator<<(std::ostream &os, const Client &client);

#endif
