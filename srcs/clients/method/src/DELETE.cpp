#include "DELETE.hpp"

DELETE::DELETE() {}
DELETE::DELETE(std::string &request) : AMethod(request) {}
DELETE::~DELETE() {}

void DELETE::doRequest(void) {
  if (std::remove(this->_path.c_str()) == false) {
    throw(this->_statusCode = NOT_FOUND);
  }
  this->_statusCode = OK;
}

void DELETE::createSuccessResponse(void) {
  this->assembleResponseLine();
  this->_responseFlag = true;
}