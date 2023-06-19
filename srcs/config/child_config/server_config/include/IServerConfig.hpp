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
  virtual std::list<ILocationConfig*> getLocationConfigs() = 0;
  virtual size_t getListen() = 0;
  virtual std::string getServerName() = 0;
  virtual std::string getErrorPage() = 0;
  virtual std::string getAccessLog() = 0;
  virtual std::string getRoot() = 0;
  virtual std::string getCgi() = 0;
};
