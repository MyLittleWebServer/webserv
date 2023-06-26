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

  static const std::string LISTEN;
  static const std::string SERVER_NAME;
  static const std::string ERROR_PAGE;
  static const std::string ACCESS_LOG;
  static const std::string ROOT;
  static const std::string CGI;

 public:
  ServerConfig();
  virtual ~ServerConfig();
  ServerConfig(const ServerConfig& src);
  ServerConfig& operator=(const ServerConfig& src);

  virtual void setVariable(const std::string& key, const std::string& value);
  virtual void addLocationConfig(ILocationConfig* location_config);

  virtual const std::string& getVariable(const std::string& key);

  std::list<ILocationConfig*> getLocationConfigs();
  short getListen();
  const std::string& getServerName();
  const std::string& getErrorPage();
  const std::string& getAccessLog();
  const std::string& getRoot();
  const std::string& getCgi();
};
