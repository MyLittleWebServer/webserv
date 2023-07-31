#include "DELETE.hpp"

DELETE::DELETE() {}
DELETE::~DELETE() {}

/**
 * @brief DELETE 메소드를 실행합니다.
 *
 * @details
 * DELETE 메소드는 파일을 삭제합니다.
 * 파일이 존재하지 않으면 404 NOT FOUND를 반환합니다.
 * 파일이 디렉토리면 403 FORBIDDEN을 반환합니다.
 * 파일이 정상적으로 삭제되면 200 OK를 반환합니다.
 *
 * @param dts
 * @param response
 *
 * @author
 * @date 2023.07.30
 */
void DELETE::doRequest(RequestDts& dts, IResponse& response) {
  (void)response;
  struct stat fileInfo;
  if (stat(dts.path->c_str(), &fileInfo) != 0)
    throw(*dts.statusCode = E_404_NOT_FOUND);
  if (fileInfo.st_mode & S_IFDIR) throw(*dts.statusCode = E_403_FORBIDDEN);
  if (std::remove(dts.path->c_str()) != 0) {
    throw(*dts.statusCode = E_404_NOT_FOUND);
  }
  *dts.statusCode = E_200_OK;
}

/**
 * @brief DELETE 메소드의 성공 응답을 생성합니다.
 *
 * @details
 * DELETE 메소드의 성공 응답은 다음과 같습니다.
 * <pre>
 * HTTP/1.1 200 OK
 * Content-Type: text/plain
 * Content-Length: 35
 *
 * Resource has been successfully deleted.
 * </pre>
 *
 * @param IResponse
 *
 * @author
 * @date 2023.07.30
 */
void DELETE::createSuccessResponse(IResponse& response) {
  response.addBody("Resource has been successfully deleted.\r\n");
  response.setHeaderField("Content-Type", "text/plain");
  response.setHeaderField("Content-Length", itos(response.getBody().size()));
  response.assembleResponse();
  response.setResponseParsed();
}
