#include "DELETE.hpp"

DELETE::DELETE() {}
DELETE::DELETE(std::string &request) : AMethod(request) {}
DELETE::~DELETE() {}

void DELETE::createResponse(void) {
  std::stringstream ss;
  ss << this->_statusCode;
  std::string deldir = this->_path;  // TODO: need to concat root as prefix
  bool isremoved = std::remove(deldir.c_str());  // return 0 or 1

  if (isremoved)
    _statusCode = 200;
  else
    _statusCode = 404;

  if (_statusCode == 200)
    this->_response += "HTTP/1.1" + ss.str() + "OK\r\n";
  else if (_statusCode == 404)
    this->_response += "HTTP/1.1" + ss.str() + "Not Found\r\n";
}