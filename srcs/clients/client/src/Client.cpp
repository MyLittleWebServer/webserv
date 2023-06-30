#include "Client.hpp"

#include "Utils.hpp"

char Client::_buf[RECEIVE_LEN + 1] = {0};

Client::Client() : _flag(START), _sd(0), _method(NULL) {}

Client::Client(const uintptr_t sd) : _flag(START), _sd(sd), _method(NULL) {}

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
  this->_flag = REQUEST_DONE;
  return (n < RECEIVE_LEN ||
          recv(this->_sd, Client::_buf, RECEIVE_LEN, MSG_PEEK) == -1);
}

void Client::receiveRequest(void) {
  while (this->_flag == START) {
    signal(SIGPIPE, SIG_IGN);
    ssize_t n = recv(this->_sd, Client::_buf, RECEIVE_LEN, 0);
    if (n <= 0) {
      signal(SIGPIPE, SIG_DFL);
      if (n == -1) throw Client::RecvFailException();
      throw Client::DisconnectedDuringRecvException();
    }
    Client::_buf[n] = '\0';
    this->_request += Client::_buf;
    std::memset(Client::_buf, 0, RECEIVE_LEN + 1);
    if (checkIfReceiveFinished(n) == true) {
#ifdef DEBUG_MSG
      std::cout << "received data from " << this->_sd << ": " << this->_request
                << std::endl;
#endif
      signal(SIGPIPE, SIG_DFL);
      return;
    }
    signal(SIGPIPE, SIG_DFL);
  }
}

void Client::sendResponse() {
  if (this->_flag != REQUEST_DONE) return;
  signal(SIGPIPE, SIG_DFL);
  if (this->_method != NULL && this->_method->getResponseFlag() == true) {
    const std::string &response = this->_method->getResponse();
    ssize_t n = send(this->_sd, response.c_str(), response.size(), 0);
    if (n <= 0) {
      if (n == -1) throw Client::SendFailException();
      signal(SIGPIPE, SIG_DFL);
      throw Client::DisconnectedDuringSendException();
    }
    this->_request.clear();
    this->_flag = END;
    signal(SIGPIPE, SIG_DFL);
    return;
  }
}

void Client::newHTTPMethod(void) {
  if (this->_request.compare(0, 4, "GET ") == 0) {
    this->_method = new GET(this->_request);
    return;
  }
  if (this->_request.compare(0, 5, "POST ") == 0) {
    this->_method = new POST(this->_request);
    return;
  }
  if (this->_request.compare(0, 7, "DELETE ") == 0) {
    this->_method = new DELETE(this->_request);
    return;
  }
  this->_method = new DummyMethod(NOT_IMPLEMENTED);
  throw(NOT_IMPLEMENTED);
}

AMethod *Client::getMethod() const { return this->_method; }

ClientFlag Client::getFlag() const { return this->_flag; }

void Client::setFlag(ClientFlag flag) { this->_flag = flag; }

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