#pragma once

#include "GET.hpp"

/**
 * @brief HEAD;
 *
 * HEAD 메소드를 구현한 클래스입니다.
 * GETD을 상속받아 구현합니다.
 * GET과 기능은 동일하지만 body를 제공하지 않습니다.
 *
 * @author middlefitting
 * @date 2023.07.21
 */
class HEAD : public GET {
 public:
  HEAD();
  ~HEAD();

  void createSuccessResponse(IResponse& response);
};
