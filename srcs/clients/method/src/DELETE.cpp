#include "DELETE.hpp"

DELETE::DELETE() {}
DELETE::~DELETE() {}

void DELETE::doRequest(RequestDts& dts, IResponse &response) {
  (void)response;
  if (std::remove(dts.path->c_str()) == false) {
    throw(*dts.statusCode = NOT_FOUND);
  }
  *dts.statusCode = OK;
}

void DELETE::createSuccessResponse(IResponse& response) {
  response.assembleResponse();
  response.setResponseParsed();
}