#include "AMethod.hpp"

AMethod::AMethod() {}
AMethod::AMethod(std::string &request)
    : _request(request), _responseFlag(false) {}
AMethod::~AMethod() {}

void AMethod::splitLinesByCRLF(void) {
  size_t pos = 0;
  size_t delimeter = this->_request.find("\r\n");
  while (delimeter != std::string::npos) {
    std::string chunk = this->_request.substr(pos, delimeter);
    this->_linesBuffer.push_back(chunk);
    pos = delimeter + 2;
    delimeter = this->_request.find("\r\n", pos);
  }
}

void AMethod::parseRequestLine(void) {
  int delim_cnt = 0;
  size_t pos = 0;
  const std::string &firstLine(this->_linesBuffer.front());
  size_t delimeter = firstLine.find(" ");
  if (delimeter == std::string::npos) {
    throw std::runtime_error("request has bad indentation!");
  }
  while (delimeter != std::string::npos) {
    delim_cnt += 1;
    pos = delimeter + 1;
    delimeter = firstLine.find(" ", pos);
  }
  if (delim_cnt != 2) throw std::runtime_error("request has bad indentation!");
  std::istringstream iss(firstLine);
  this->_linesBuffer.pop_front();
  iss >> this->_method >> this->_path >> this->_protocol;
}

void AMethod::parseHeaderFields(void) {
  std::list<std::string>::const_iterator it = this->_linesBuffer.begin();
  std::list<std::string>::const_iterator end = this->_linesBuffer.end();

  std::string key;
  std::string value;

  size_t pos = 0;
  while (it != end) {
    pos = (*it).find(": ");
    key = (*it).substr(0, pos);
    value = (*it).substr(pos + 2, (*it).size() - pos - 2);
    this->_linesBuffer.pop_front();
    this->_headerFields.insert(std::pair<std::string, std::string>(key, value));
    ++it;
    if (checkBodyExistance(*it)) {
      this->_linesBuffer.pop_front();
      return;
    }
  }
}

bool AMethod::checkBodyExistance(const std::string &str) const {
  return (str == "");
}

const std::string &AMethod::getResponse(void) const {
  return (this->_response);
}

bool AMethod::getResponseFlag(void) const { return (this->_responseFlag); }
