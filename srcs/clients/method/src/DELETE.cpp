#include "DELETE.hpp"

DELETE::DELETE() {}
DELETE::~DELETE() {}

void DELETE::doRequest(RequestDts& dts, IResponse &response) {
  (void)response;
  if (std::remove(dts.path->c_str()) == false) {
    throw(*dts.statusCode = E_404_NOT_FOUND);
  }
  *dts.statusCode = E_200_OK;
}

void DELETE::createSuccessResponse(IResponse& response) {
  response.assembleResponse();
  response.setResponseParsed();
}