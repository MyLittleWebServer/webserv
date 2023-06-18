#pragma once

#include "../../include/IChildConfig.hpp"
#include "../../location_config/include/LocationConfig.hpp"

/**
 * @brief Server 설정 파일 정보를 제공하는 인터페이스
 * @author middlefitting
 * @date 2023.06.17
 */
class IServerConfig : public IChildConfig {
 public:
  virtual void addLocationConfig(ILocationConfig* location_config) = 0;
};
