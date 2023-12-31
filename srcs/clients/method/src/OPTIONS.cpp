#include "OPTIONS.hpp"

OPTIONS::OPTIONS() {}

OPTIONS::~OPTIONS() {}

OPTIONS::OPTIONS(const OPTIONS& src) { *this = src; }

OPTIONS& OPTIONS::operator=(const OPTIONS& src) {
  (void)src;
  return *this;
}

/**
 * @brief OPTION 메소드를 실행합니다.
 *
 * *dts.path == "*" 경우 전체 서버의 사용가능한 메소드의 전체 옵션을 보여줍니다.
 * 그 외의 경우 해당 location의 옵션을 보여줍니다.
 *
 * @param RequestDts HTTP 관련 데이터
 * @param IResponse 응답 객체
 *
 * @author
 * @date 2023.07.29
 */
void OPTIONS::doRequest(RequestDts& dts, IResponse& response) {
  if (*dts.path == "*") {
    makeServerOptions(response);
  } else {
    makeLocationOption(dts, response);
  }
  response.setHeaderField("Server", (*dts.matchedServer)->getServerName());
}

/**
 * @brief 서버에 구현된 메소드 옵션을 response 객체에 추가합니다.
 *
 * @param IResponse 응답 객체
 *
 * @author
 * @date 2023.07.29
 */
void OPTIONS::makeServerOptions(IResponse& response) {
  response.setHeaderField("Allow", "GET, HEAD, POST, PUT, DELETE, OPTIONS");
}

/**
 * @brief location에 구현된 메소드 옵션을 response 객체에 추가합니다.
 *
 * @param RequestDts HTTP 관련 데이터
 * @param IResponse 응답 객체
 *
 * @author
 * @date 2023.07.29
 */
void OPTIONS::makeLocationOption(RequestDts& dts, IResponse& response) {
  std::string allow_methods =
      (*dts.matchedLocation)->getVariable("allow_method");
  response.setHeaderField("Allow", allow_methods);
}

/**
 * @brief RFC OPTIONS의 맞게 응답 객체를 설정합니다. 응답 객체를 파싱합니다.
 *
 * @param IResponse 응답 객체
 *
 * @author
 * @date 2023.07.29
 */
void OPTIONS::createSuccessResponse(IResponse& response) {
  response.setStatusCode(E_200_OK);
  response.setHeaderField("Content-Length", "0");
  response.setBody("");
  response.assembleResponse();
  response.setResponseParsed();
}
