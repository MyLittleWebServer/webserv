#include "Client.hpp"

#include "DELETE.hpp"
#include "DummyMethod.hpp"
#include "GET.hpp"
#include "Kqueue.hpp"
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

void Client::receiveRequest(void) {
  _state = RECEIVING;
  while (true) {
    ssize_t n = recv(_sd, Client::_buf, RECEIVE_LEN, 0);
    if (n <= 0) {
      if (n == -1) throw Client::RecvFailException();
      throw Client::DisconnectedDuringRecvException();
    }
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

void Client::removeTimeOutEventInEventsToAdd(
    std::vector<struct kevent> &_eventsToAdd) {
  _state = METHOD_SELECT;
  _eventsToAdd.pop_back();
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
  if (_request.isParsed()) _state = REQUEST_DONE;
}

bool Client::isCgi() { return _request.isCgi(); }

void Client::doRequest() {
  _method->doRequest(_request.getRequestParserDts(), _response);
}

void Client::sendResponse() {
  const std::string &response = _response.getResponse();
  ssize_t n = send(_sd, response.c_str() + _lastSentPos,
                   response.size() - _lastSentPos, 0);
  if (n <= 0) {
    if (n == -1) throw Client::SendFailException();
    throw Client::DisconnectedDuringSendException();
  }
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
}

IMethod *Client::getMethod() const { return _method; }

ClientStates Client::getState() const { return _state; }

void Client::setState(ClientStates state) { _state = state; }

uintptr_t Client::getSD() const { return _sd; }

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

void Client::setResponseConnection() {
  if (_request.getHeaderField("connection") == "close") {
    _response.setHeaderField("Connection", "close");
  } else {
    _response.setHeaderField("Connection", "keep-alive");
  }
  _response.assembleResponse();
  _state = PROCESS_RESPONSE;
}

void Client::setConnectionClose() {
  _request.setHeaderField("Connection", "close");
}