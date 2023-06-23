#pragma once

#include "../../include/IChildConfig.hpp"

/**
 * @brief Root 설정 파일 정보를 제공하는 인터페이스
 * @author middlefitting
 * @date 2023.06.16
 */
class IRootConfig : public IChildConfig {
 public:
  const virtual std::string& getUser() = 0;
  virtual size_t getWorkerProcesses() = 0;
};
