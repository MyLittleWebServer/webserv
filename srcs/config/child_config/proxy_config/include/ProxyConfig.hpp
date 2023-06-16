#pragma once

#include <map>

#include "IProxyConfig.hpp"

/**
 * @brief Proxy 설정 파일 정보를 제공하는 클래스
 * @author middlefitting
 * @date 2023.06.17
 */
class ProxyConfig : public IProxyConfig {
 private:
  std::map<std::string, unsigned int> _data;

 public:
  ProxyConfig();
  virtual ~ProxyConfig();
  ProxyConfig(const ProxyConfig& src);
  ProxyConfig& operator=(const ProxyConfig& src);

  virtual void setVariable(const std::string& key, const std::string& value);
};
