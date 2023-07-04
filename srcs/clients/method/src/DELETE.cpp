#include "DELETE.hpp"

DELETE::DELETE() {}
DELETE::~DELETE() {}

void DELETE::doRequest(RequestDts& dts) {
  if (std::remove(dts.path->c_str()) == false) {
    throw(*dts.statusCode = NOT_FOUND);
  }
  *dts.statusCode = OK;
}

void DELETE::createSuccessResponse(IResponse& response) {
  response.assembleResponseLine();
  response.setResponseParsed();
}