#include "DELETE.hpp"

DELETE::DELETE() {}
DELETE::~DELETE() {}

void DELETE::doRequest(RequestDts& dts, IResponse& response) {
  (void)response;
  if (std::remove(dts.path->c_str()) != 0) {
    std::cout << *dts.path << '\n';
    throw(*dts.statusCode = E_404_NOT_FOUND);
  }
  *dts.statusCode = E_200_OK;
}

void DELETE::createSuccessResponse(IResponse& response) {
  response.addBody("Resource has been successfully deleted.\r\n");
  response.setHeaderField("Content-Type", "text/plain");
  response.setHeaderField("Content-Length", itos(response.getBody().size()));
  response.assembleResponse();
  response.setResponseParsed();
}