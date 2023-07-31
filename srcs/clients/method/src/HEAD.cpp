#include "HEAD.hpp"

HEAD::HEAD() : GET() {}
HEAD::~HEAD() {}

/**
 * @brief HEAD 메소드를 처리합니다.
 *
 * @details
 * HEAD 는 GET과 동일한 동작을 하지만, body를 제외한 response를 생성합니다.
 * 따라서, GET에서 진행한 작업에서 body를 제외한 response를 생성합니다.
 *
 * @param IResponse 응답 객체
 *
 * @author
 * @date 2023.07.29
 */
void HEAD::createSuccessResponse(IResponse& response) {
  validateContentType(response);
  response.setStatusCode(E_204_NO_CONTENT);
  response.setHeaderField("Content-Length", itos(response.getBody().size()));
  response.setBody("");
  response.assembleResponse();
  response.setResponseParsed();
}
