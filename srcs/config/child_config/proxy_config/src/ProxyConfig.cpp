#include "../include/ProxyConfig.hpp"

ProxyConfig::ProxyConfig() {
  _data.insert(std::pair<std::string, unsigned int>("CLIENT_MAX_BODY_SIZE",
                                                    CLIENT_MAX_BODY_SIZE));
  _data.insert(std::pair<std::string, unsigned int>("REQUEST_URI_LIMIT_SIZE",
                                                    REQUEST_URI_LIMIT_SIZE));
  _data.insert(std::pair<std::string, unsigned int>("REQUEST_HEADER_LIMIT_SIZE",
                                                    REQUEST_HEADER_LIMIT_SIZE));
}

ProxyConfig::~ProxyConfig() {}

ProxyConfig::ProxyConfig(const ProxyConfig& src) { *this = src; }

ProxyConfig& ProxyConfig::operator=(const ProxyConfig& src) {
  if (this != &src) {
    _data = src._data;
  }
  return *this;
}

void ProxyConfig::setVariable(const std::string& key,
                              const std::string& value) {
  _data.insert(
      std::pair<std::string, unsigned int>(key, std::atoi(value.c_str())));
}

std::string ProxyConfig::getVariable(const std::string& key) {
  std::map<std::string, size_t>::iterator it = _data.find(key);
  if (it != _data.end()) {
    return std::to_string(it->second);
  }
  throw ExceptionThrower::InvalidConfigException(NOT_SUPPORT_CONFIG);
}

std::size_t ProxyConfig::getClientMaxBodySize() {
  return std::atoi(getVariable("CLIENT_MAX_BODY_SIZE").c_str());
}

std::size_t ProxyConfig::getRequestUriLimitSize() {
  return std::atoi(getVariable("REQUEST_URI_LIMIT_SIZE").c_str());
}

std::size_t ProxyConfig::getRequestHeaderLimitSize() {
  return std::atoi(getVariable("REQUEST_HEADER_LIMIT_SIZE").c_str());
}