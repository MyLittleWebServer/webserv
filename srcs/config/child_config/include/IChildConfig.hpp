#pragma once

#include <iostream>

#include "../../../exception/include/ExceptionThrower.hpp"

/**
 * @brief 자식 설정 파일 공통 인터페이스
 * @author middlefitting
 * @date 2023.06.16
 */
class IChildConfig {
 public:
  virtual void setVariable(const std::string& key,
                           const std::string& value) = 0;

  virtual std::string getVariable(const std::string& key) = 0;
};
