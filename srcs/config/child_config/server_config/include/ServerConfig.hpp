#pragma once

#include <list>
#include <map>

#include "IServerConfig.hpp"

/**
 * @brief Server 설정 파일 정보를 제공하는 클래스
 * @author middlefitting
 * @date 2023.06.17
 */
class ServerConfig : public IServerConfig {
 private:
  std::map<std::string, std::string> _data;
  std::list<ILocationConfig*> _location_configs;

 public:
  ServerConfig();
  virtual ~ServerConfig();
  ServerConfig(const ServerConfig& src);
  ServerConfig& operator=(const ServerConfig& src);

  virtual void setVariable(const std::string& key, const std::string& value);
  virtual void addLocationConfig(ILocationConfig* location_config);
};
