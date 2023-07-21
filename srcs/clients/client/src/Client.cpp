#include "Client.hpp"

#include "DELETE.hpp"
#include "DummyMethod.hpp"
#include "GET.hpp"
#include "HEAD.hpp"
#include "Kqueue.hpp"
#include "OPTIONS.hpp"
#include "POST.hpp"
#include "Utils.hpp"

char Client::_buf[RECEIVE_LEN] = {0};

Client::Client() : _state(START), _sd(0), _method(NULL) {
  _method = NULL;
  _cgi = NULL;
  _lastSentPos = 0;
}

Client::Client(const uintptr_t sd) {
  _state = START;
  _sd = sd;
  _method = NULL;
  _cgi = NULL;
  _lastSentPos = 0;
}

Client &Client::operator=(const Client &client) {
  _state = client._state;
  _sd = client._sd;
  _request = client._request;
  _method = client._method;
  _lastSentPos = client._lastSentPos;
  return *this;
}

Client::~Client(void) {
#ifdef DEBUG_MSG
  std ::cout << " Client destructor called " << getSD() << " !" << std::endl;
#endif
  close(_sd);
  if (_method != NULL) delete _method;
  if (_cgi != NULL) delete _cgi;
}

bool Client::checkIfReceiveFinished(ssize_t n) {
  return (n < RECEIVE_LEN ||
          recv(_sd, Client::_buf, RECEIVE_LEN, MSG_PEEK) == -1);
}

/**
 * @brief 클라이언트가 요청을 수신합니다.
 *
 * @details
 * 1. 클라이언트의 현재 상태를 RECEIVING으로 설정합니다.
 * 2. recv를 통해 client의 정적 변수 _buf에 수신 데이터를 저장합니다.
 * 4. recv로 받은 n의 값이 0보다 작거나 같으면 예외를 발생시킵니다.
 *  이는 수신 중 오류가 발생했거나 클라이언트가 연결을 끊었음을 나타냅니다.
 * 5. 수신된 데이터를 _recvBuff에 추가합니다.
 * 6. _buf를 다시 0으로 초기화합니다.
 * 7. 요청을 완전히 수신했는지 checkIfReceiveFinished를 사용하여 확인합니다.
 * 만약 완전히 수신되었으면, 루프를 탈출합니다.
 *
 * @exception Client::RecvFailException
 * recv 함수가 -1을 반환하면 발생하는 예외입니다. 이는 수신 중 오류가 발생했음을
 * 나타냅니다.
 * @exception Client::DisconnectedDuringRecvException
 * recv 함수가 0을 반환하면 발생하는 예외입니다. 이는 클라이언트가 요청 수신 중
 * 연결을 끊었음을 나타냅니다.
 */
void Client::receiveRequest(void) {
  _state = RECEIVING;
  while (true) {
    ssize_t n = recv(_sd, Client::_buf, RECEIVE_LEN, 0);
    if (n == -1)
      throw Client::RecvFailException();
    else if (n <= 0)
      throw Client::DisconnectedDuringRecvException();

    _recvBuff.insert(_recvBuff.end(), Client::_buf, Client::_buf + n);
    std::memset(Client::_buf, 0, RECEIVE_LEN);
    if (checkIfReceiveFinished(n) == true) {
#ifdef DEBUG_MSG
      std::cout << "received data from " << _sd << ": " << _request
                << std::endl;
#endif
      break;
    }
  }
}

void Client::createExceptionResponse() {
  _response.createExceptionResponse(_request.getRequestParserDts());
}

void Client::createExceptionResponse(Status statusCode) {
  *_request.getRequestParserDts().statusCode = statusCode;
  _response.createExceptionResponse(_request.getRequestParserDts());
}

void Client::createSuccessResponse() {
  _method->createSuccessResponse(_response);
}

void Client::parseRequest(short port) {
  if (_request.isParsed()) return;
  _request.parseRequest(_recvBuff, port);
  if (_request.isParsed()) _state = METHOD_SELECT;
}

bool Client::isCgi() { return _request.isCgi(); }

void Client::doRequest() {
  _method->doRequest(_request.getRequestParserDts(), _response);
}
/**
 * @brief 클라이언트가 응답을 전송합니다.
 *
 * @details
 * 1. Response객체로 부터 전송할 데이터의 response를 받아옵니다.
 * 2. send를 통해 SD로 response의 데이터를 로직에 맞춰 전송합니다.
 * 3. send 함수의 오류를 처리합니다.
 * 4. send로 보낸 n의 값이 (response의 크기 - 마지막으로 보낸 위치)와 다르다면
 * 현재 함수에서 탈출합니다.
 * 5. Request의 connection 헤더 필드가 close라면 state를 END_CLOSE로 설정합니다.
 * 6. 그렇지 않다면 state를 END_KEEP_ALIVE로 설정합니다.
 *
 * @exception Client::SendFailException
 * send 함수가 -1을 반환하면 발생하는 예외입니다. 이는 전송 중 오류가 발생했음을
 * 나타냅니다.
 * @exception Client::DisconnectedDuringSendException
 * send 함수가 0을 반환하면 발생하는 예외입니다. 이는 클라이언트가 응답 전송 중
 * 연결을 끊었음을 나타냅니다.
 */
void Client::sendResponse() {
  const std::string &response = _response.getResponse();
  ssize_t n = send(_sd, response.c_str() + _lastSentPos,
                   response.size() - _lastSentPos, 0);
  if (n == -1) throw Client::SendFailException();
  if (n <= 0) throw Client::DisconnectedDuringSendException();

  if (static_cast<size_t>(n) != response.size() - _lastSentPos) {
    _lastSentPos += n;
    return;
  }
  if (_request.getHeaderField("connection") == "close") {
    _state = END_CLOSE;
    return;
  }
  _state = END_KEEP_ALIVE;
}

void Client::newHTTPMethod(void) {
  if (_request.getMethod() == "GET")
    _method = new GET();
  else if (_request.getMethod() == "POST")
    _method = new POST();
  else if (_request.getMethod() == "DELETE")
    _method = new DELETE();
  else if (_request.getMethod() == "HEAD")
    _method = new HEAD();
  else if (_request.getMethod() == "OPTIONS")
    _method = new OPTIONS();
}

IMethod *Client::getMethod() const { return _method; }

ClientStates Client::getState() const { return _state; }

void Client::setState(ClientStates state) { _state = state; }

/**
 * @brief 소켓 디스크립터를 반환합니다.
 *
 * @return uintptr_t
 */
uintptr_t Client::getSD() const { return this->_sd; }

const char *Client::RecvFailException::what() const throw() {
  return ("error occured in recv()");
}

const char *Client::DisconnectedDuringRecvException::what() const throw() {
  return ("client disconnected while recv()");
}

const char *Client::SendFailException::what() const throw() {
  return ("error occured in send()");
}

const char *Client::DisconnectedDuringSendException::what() const throw() {
  return ("client disconnected while send()");
}

std::ostream &operator<<(std::ostream &os, const Client &client) {
  os << "Client: " << client.getSD() << std::endl;
  return os;
}

void Client::makeAndExecuteCgi() {
  _cgi = new CGI(&_request, &_response, _sd, static_cast<void *>(this));
  _cgi->executeCGI();
}

void Client::clear() {
  _state = START;
  _lastSentPos = 0;
  _recvBuff.clear();

  _request.clear();
  _response.clear();

  if (_cgi != NULL) {
    delete _cgi;
    _cgi = NULL;
  }
  if (_method != NULL) {
    delete _method;
    _method = NULL;
  }
}

/**
 * @brief 연결 응답을 설정합니다.
 *
 * @details
 * 1. Request의 connection 헤더 필드가 close라면 Response의 connection 헤더
 * 필드도 close로 설정합니다.
 * 2. Request의 connection 헤더 필드가 명시 되어있지 않다면 Response의
 * connection 헤더 필드도 keep-alive로 설정합니다.
 *
 */
void Client::setResponseConnection() {
  if (_request.getHeaderField("connection") == "close")
    _response.setHeaderField("connection", "close");
  else
    _response.setHeaderField("connection", "keep-alive");
}

/**
 * @brief setConnectionClose;
 *
 * Connection을 close로 설정하여 서버 스스로 강제로 소켓 연결을
 * 끊습니다.
 *
 * @param RequestDts HTTP 관련 데이터
 * @return void
 * @author middlefitting
 * @date 2023.07.21
 */
void Client::setConnectionClose() {
  _request.setHeaderField("connection", "close");
}

/**
 * @brief bodyCheck;
 *
 * Method가 HEAD 라면 BODY를 제거합니다.
 *
 * @param RequestDts HTTP 관련 데이터
 * @return void
 * @author middlefitting
 * @date 2023.07.21
 */
void Client::bodyCheck() {
  if (_request.getMethod() == "HEAD") _response.setBody("");
}

/**
 * @brief ressembleResponse;
 *
 * Response를 조립합니다.
 * state를 PROCESS_RESPONSE로 변경합니다.
 * 최종적으로 마지막 파서를 거쳐 Response를 보낼 준비가 돠었다는 것을
 * 의미합니다.
 *
 * @param RequestDts HTTP 관련 데이터
 * @return void
 * @author middlefitting
 * @date 2023.07.21
 */
void Client::reassembleResponse() {
  _response.assembleResponse();
  _state = PROCESS_RESPONSE;
}
