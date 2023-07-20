#include "HEAD.hpp"

HEAD::HEAD() : GET() {}
HEAD::~HEAD() {}

void HEAD::createSuccessResponse(IResponse& response) {
  validateContentType(response);
  response.setStatusCode(E_204_NO_CONTENT);
  response.setHeaderField("Date", getCurrentTime());
  response.setHeaderField("Content-Length", itos(response.getBody().size()));
  response.setBody("");
  response.assembleResponse();
#ifdef DEBUG_MSG_BODY
  std::cout << response.getResponse() << "\n";
#endif
  response.setResponseParsed();
}
