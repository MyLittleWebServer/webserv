#include "DELETE.hpp"

DELETE::DELETE() {}
DELETE::~DELETE() {}

void DELETE::doRequest(RequestDts& dts, IResponse &response, FdInfo &fdInfo) {
  (void)response;
  (void)fdInfo;
  if (std::remove(dts.path->c_str()) == false) {
    throw(*dts.statusCode = NOT_FOUND);
  }
  *dts.statusCode = OK;
}

void DELETE::createSuccessResponse(IResponse& response) {
  response.assembleResponse();
  response.setResponseParsed();
}