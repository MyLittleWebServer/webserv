#pragma once

#include <iostream>

#include "../parser/include/ConfigParser.hpp"

/**
 * @brief 설정 파일 정보를 제공하는 인터페이스
 * @author middlefitting
 * @date 2023.06.15
 */
class IConfig {
 public:
  virtual IRootConfig& getRootConfig() = 0;
  virtual IProxyConfig& getProxyConfig() = 0;
  virtual IMimeTypesConfig& getMimeTypesConfig() = 0;
  virtual std::list<IServerConfig*>& getServerConfigs() = 0;
};
