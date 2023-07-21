#include "HEAD.hpp"

HEAD::HEAD() : GET() {}
HEAD::~HEAD() {}

/**
 * @brief createSuccessResponse;
 *
 * GET에서 진행한 작업에서 body를 제외한 response를 생성합니다.
 *
 * @param IResponse 응답 객체
 *
 * @return void
 *
 * @author middlefitting
 * @date 2023.07.21
 */
void HEAD::createSuccessResponse(IResponse& response) {
  validateContentType(response);
  response.setStatusCode(E_204_NO_CONTENT);
  response.setHeaderField("Date", getCurrentTime());
  response.setHeaderField("Content-Length", itos(response.getBody().size()));
  response.setBody("");
  response.assembleResponse();
  response.setResponseParsed();
}
