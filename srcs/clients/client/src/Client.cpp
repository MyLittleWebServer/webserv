#include "Client.hpp"

#include "Utils.hpp"

char Client::_buf[RECEIVE_LEN + 1] = {0};

Client::Client() : _sd(0), _method(NULL) {}

Client::Client(const uintptr_t sd) : _sd(sd), _method(NULL) {}

Client::~Client(void) {
  std ::cout << "Client destructor called" << *this << "!!!!" << std::endl;
  if (this->_method != NULL) delete this->_method;
}

bool Client::checkIfReceiveFinished(ssize_t n) const {
  return (n < RECEIVE_LEN ||
          recv(this->_sd, Client::_buf, RECEIVE_LEN, MSG_PEEK) == -1);
}

void Client::receiveRequest(void) {
  while (1) {
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
      std::cout << "received data from " << this->_sd << ": " << this->_request
                << std::endl;
      signal(SIGPIPE, SIG_DFL);
      return;
    }
    signal(SIGPIPE, SIG_DFL);
  }
}

void Client::sendResponse(std::map<int, Client *> &clients) {
  if (clients.empty()) return;
  std::map<int, Client *>::iterator it = clients.find(this->_sd);
  if (it != clients.end() && this->_method != NULL &&
      this->_method->getResponseFlag() == true) {
    const std::string &response = this->_method->getResponse();
    ssize_t n = send(this->_sd, response.c_str(), response.size(), 0);
    if (n == -1) {
      throw std::runtime_error("Client send error!");
    }
    this->_request.clear();
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

const char *Client::RecvFailException::what() const throw() {
  return ("error occured in recv()");
}

const char *Client::DisconnectedDuringRecvException::what() const throw() {
  return ("client disconnected while recv()");
}

uintptr_t Client::getSD() const { return this->_sd; }

std::ostream &operator<<(std::ostream &os, const Client &client) {
  os << "Client: " << client.getSD() << std::endl;
  return os;
}