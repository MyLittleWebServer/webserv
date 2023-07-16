#include "Client.hpp"

#include "DELETE.hpp"
#include "DummyMethod.hpp"
#include "GET.hpp"
#include "POST.hpp"
#include "Utils.hpp"

char Client::_buf[RECEIVE_LEN + 1] = {0};

Client::Client() : _flag(START), _sd(0), _method(NULL) {
  this->_method = NULL;
  this->_cgi = NULL;
}

Client::Client(const uintptr_t sd) {
  this->_flag = RECEIVING;
  this->_sd = sd;
  this->_method = NULL;
  this->_cgi = NULL;
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
  close(_sd);
  if (_method != NULL) delete _method;
  if (_cgi != NULL) delete _cgi;
}

bool Client::checkIfReceiveFinished(ssize_t n) {
  return (n < RECEIVE_LEN ||
          recv(this->_sd, Client::_buf, RECEIVE_LEN, MSG_PEEK) == -1);
}

void Client::receiveRequest(void) {
  while (true) {
    ssize_t n = recv(this->_sd, Client::_buf, RECEIVE_LEN, 0);
    if (n <= 0) {
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
      break;
    }
  }
}

void Client::createErrorResponse() {
  _response.createErrorResponse(_request.getRequestParserDts());
}

void Client::createErrorResponse(Status statusCode) {
  *_request.getRequestParserDts().statusCode = statusCode;
  _response.createErrorResponse(_request.getRequestParserDts());
}

void Client::createSuccessResponse() {
  _method->createSuccessResponse(_response);
}

void Client::parseRequest(short port) {
  if (_request.isParsed()) return;
  _request.parseRequest(_recvBuff, port);
  if (_request.isParsed()) _flag = REQUEST_DONE;
}

bool Client::isCgi() { return _request.isCgi(); }

void Client::doRequest() {
  this->_method->doRequest(_request.getRequestParserDts(), _response);
}

void Client::sendResponse() {
  const std::string &response = _response.getResponse();
  ssize_t n = send(_sd, response.c_str(), response.size(), 0);
  if (n <= 0) {
    if (n == -1) throw Client::SendFailException();
    throw Client::DisconnectedDuringSendException();
  }
  if (n != static_cast<ssize_t>(response.size())) {
    _response.setResponse(response.substr(n));
    return;
  }
  _flag = END_KEEP_ALIVE;
  if (_request.getHeaderField("connection") == "close") _flag = END_CLOSE;
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
  /*
  this->_method = new DummyMethod(E_501_NOT_IMPLEMENTED);
  throw(E_501_NOT_IMPLEMENTED);
  */
}

IMethod *Client::getMethod() const { return this->_method; }

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

void Client::makeAndExecuteCgi() {
  _cgi = new CGI(&_request, &_response, _sd, static_cast<void *>(this));
  _cgi->executeCGI();
}

void Client::clear() {
  _flag = RECEIVING;
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
