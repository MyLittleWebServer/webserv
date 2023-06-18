#include "../include/ProxyConfig.hpp"

ProxyConfig::ProxyConfig() {}

ProxyConfig::~ProxyConfig() {}

ProxyConfig::ProxyConfig(const ProxyConfig& src) { *this = src; }

ProxyConfig& ProxyConfig::operator=(const ProxyConfig& src) {
  if (this != &src) {
    // do something
  }
  return *this;
}

void ProxyConfig::setVariable(const std::string& key,
                              const std::string& value) {
  _data.insert(
      std::pair<std::string, unsigned int>(key, std::atoi(value.c_str())));
}
