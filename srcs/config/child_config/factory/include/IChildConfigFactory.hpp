#pragma once

#include <iostream>
#include <list>

#include "../../../parser/include/ConfigParser.hpp"
#include "../../include/IChildConfig.hpp"
#include "../../mime_types_config/include/MimeTypesConfig.hpp"
#include "../../proxy_config/include/ProxyConfig.hpp"
#include "../../root_config/include/RootConfig.hpp"
#include "../../server_config/include/ServerConfig.hpp"

/**
 * @brief ChildConfig 의 생성을 담당하는 인터페이스
 * @author middlefitting
 * @date 2023.06.17
 */
class IChildConfigFactory {
 public:
  virtual std::list<IServerConfig*> generateServers(
      const std::string& config_data) = 0;
};
