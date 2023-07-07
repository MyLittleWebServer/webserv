#include "Client.hpp"

#include "DELETE.hpp"
#include "DummyMethod.hpp"
#include "GET.hpp"
#include "POST.hpp"
#include "Utils.hpp"

char Client::_buf[RECEIVE_LEN + 1] = {0};

Client::Client() : _flag(RECEIVING), _sd(0), _method(NULL) {}

Client::Client(const uintptr_t sd) {
  this->_flag = RECEIVING;
  this->_sd = sd;
  this->_method = NULL;
}

Client &Client::operator=(const Client &client) {
  this->_flag = client._flag;
  this->_sd = client._sd;
  this->_request = client._request;
  this->_method = client._method;
  return *this;
}

Client::~Client(void) {
#ifdef DEBUG_MSG
  std ::cout << " Client destructor called " << this->getSD() << " !"
             << std::endl;
#endif
}

bool Client::checkIfReceiveFinished(ssize_t n) {
  return (n < RECEIVE_LEN ||
          recv(this->_sd, Client::_buf, RECEIVE_LEN, MSG_PEEK) == -1);
}

void Client::receiveRequest(void) {
  while (this->_flag == RECEIVING) {
    signal(SIGPIPE, SIG_IGN);
    ssize_t n = recv(this->_sd, Client::_buf, RECEIVE_LEN, 0);
    if (n <= 0) {
      signal(SIGPIPE, SIG_DFL);
      if (n == -1) throw Client::RecvFailException();
      throw Client::DisconnectedDuringRecvException();
    }
    Client::_buf[n] = '\0';
    this->_recvBuff += Client::_buf;
    std::memset(Client::_buf, 0, RECEIVE_LEN + 1);
    if (checkIfReceiveFinished(n) == true) {
#ifdef DEBUG_MSG
      std::cout << "received data from " << this->_sd << ": " << this->_request
                << std::endl;
#endif
      this->_flag = RECEIVE_DONE;
      signal(SIGPIPE, SIG_DFL);
      return;
    }
    signal(SIGPIPE, SIG_DFL);
  }
}

void Client::createErrorResponse() { _response.createErrorResponse(); }
void Client::createSuccessResponse() {
  _method->createSuccessResponse(_response);
}

void Client::parseRequest(short port) {
  if (_request.isParsed()) return;
  _request.parseRequest(_recvBuff, port);
  if (_request.isParsed()) _flag = REQUEST_PARSED;
}

bool Client::isCgi() { return _request.isCgi(); }

void Client::doRequest() {
  this->_method->doRequest(_request.getRequestParserDts(), _response, _fdInfo);
}

void Client::sendResponse() {
  signal(SIGPIPE, SIG_DFL);
  // const std::string &response = this->_method->getResponse();
  const std::string &response = _response.getResponse();

  ssize_t n = send(this->_sd, response.c_str(), response.size(), 0);

  if (n <= 0) {
    if (n == -1) throw Client::SendFailException();
    signal(SIGPIPE, SIG_DFL);
    throw Client::DisconnectedDuringSendException();
  }
  // need to make _request.clear(), _response.clear() logic
  this->_flag = END;
  signal(SIGPIPE, SIG_DFL);
  return;
}

void Client::newHTTPMethod(void) {
  if (this->_request.getMethod() == "GET") {
    this->_method = new GET();
    return;
  }
  if (this->_request.getMethod() == "POST") {
    this->_method = new POST();
    return;
  }
  if (this->_request.getMethod() == "DELETE") {
    this->_method = new DELETE();
    return;
  }
  this->_method = new DummyMethod(NOT_IMPLEMENTED);
  throw(NOT_IMPLEMENTED);
}

IMethod *Client::getMethod() const { return this->_method; }

ClientFlag Client::getFlag() const { return this->_flag; }

void Client::setFlag(ClientFlag flag) { this->_flag = flag; }

uintptr_t Client::getSD() const { return this->_sd; }

const FdInfo &Client::getFdInfo() const { return this->_fdInfo; };

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
