#pragma once

#include "IMethod.hpp"

/**
 * @brief OPTIONS;
 *
 * OPTIONS 메소드를 구현한 클래스입니다.
 * OPTIONS 메소드는 서버에 구현된 메소드 옵션을 보여줍니다.
 * IMethod를 상속받아 구현합니다.
 *
 * @author middlefitting
 * @date 2023.07.21
 */
class OPTIONS : public IMethod {
 private:
  void makeServerOptions(IResponse& response);
  void makeLocationOption(RequestDts& dts, IResponse& response);

 public:
  OPTIONS();
  virtual ~OPTIONS();
  OPTIONS(const OPTIONS& src);
  OPTIONS& operator=(const OPTIONS& src);

  void doRequest(RequestDts& dts, IResponse& response);
  void createSuccessResponse(IResponse& response);
};
