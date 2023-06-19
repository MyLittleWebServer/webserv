#include "Client.hpp"

char Client::_buf[TRANSFER_LEN + 1] = {0};

Client::Client() : _sd(0), _method(NULL) {}

Client::Client(const uintptr_t sd) : _sd(sd), _method(NULL) {}

Client::~Client(void) {
  if (this->_method != NULL) delete this->_method;
}

void Client::receiveData(void) {
  while (1) {
    ssize_t n = recv(this->_sd, Client::_buf, TRANSFER_LEN, 0);
    // std::cout << "n: " << n << '\n'; zoosuck
    if (n <= 0) {
      if (n < 0) throw std::runtime_error("Client read error!");
      throw std::runtime_error("Client disconnected!");
    }
    Client::_buf[n] = '\0';
    this->_request += Client::_buf;
    std::memset(Client::_buf, 0, TRANSFER_LEN + 1);
    if (n < TRANSFER_LEN ||
        recv(this->_sd, Client::_buf, TRANSFER_LEN, MSG_PEEK) == -1) {
      std::cout << "received data from " << this->_sd << ": " << this->_request
                << std::endl;
      return;
    }
  }
}

void Client::makeResponse(void) {
  this->_method->parseRequest();
  this->_method->createResponse();
}

void Client::sendData(std::map<int, Client> &clients) {
  std::map<int, Client>::iterator it = clients.find(this->_sd);
  if (it != clients.end() && this->_method != NULL &&
      this->_method->getResponseFlag() == true) {
    const std::string &response = this->_method->getResponse();
    std::string::const_iterator pos = response.begin();
    std::string::const_iterator begin = response.begin();
    std::string::const_iterator end = response.end();
    while (1) {
      ssize_t n =
          send(this->_sd, response.c_str() + (pos - begin), TRANSFER_LEN, 0);

      if (n == -1) throw std::runtime_error("Client send error!");
      pos += n;
      if (pos == end) {
        this->_request.clear();
        delete this->_method;
        this->_method = NULL;
        return;
      }
    }
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
  throw std::runtime_error("501 Not implemented");
}