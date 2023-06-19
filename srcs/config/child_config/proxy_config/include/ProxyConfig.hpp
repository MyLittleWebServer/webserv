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
  std::map<std::string, size_t> _data;

  static const size_t CLIENT_MAX_BODY_SIZE = 1000000;
  static const size_t REQUEST_URI_LIMIT_SIZE = 1024;
  static const size_t REQUEST_HEADER_LIMIT_SIZE = 256;

 public:
  ProxyConfig();
  virtual ~ProxyConfig();
  ProxyConfig(const ProxyConfig& src);
  ProxyConfig& operator=(const ProxyConfig& src);

  virtual void setVariable(const std::string& key, const std::string& value);

  std::string getVariable(const std::string& key);

  virtual std::size_t getClientMaxBodySize();
  virtual std::size_t getRequestUriLimitSize();
  virtual std::size_t getRequestHeaderLimitSize();
};
