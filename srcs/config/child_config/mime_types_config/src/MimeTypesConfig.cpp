#include "../include/MimeTypesConfig.hpp"

MimeTypesConfig::MimeTypesConfig() {}

MimeTypesConfig::~MimeTypesConfig() {}

MimeTypesConfig::MimeTypesConfig(const MimeTypesConfig& src) { *this = src; }

MimeTypesConfig& MimeTypesConfig::operator=(const MimeTypesConfig& src) {
  if (this != &src) {
    // do something
  }
  return *this;
}

void MimeTypesConfig::setVariable(const std::string& key,
                                  const std::string& value) {
  _data.insert(std::pair<std::string, std::string>(key, value));
}

std::string MimeTypesConfig::getVariable(const std::string& key) {
  std::map<std::string, std::string>::iterator it = _data.find(key);
  if (it != _data.end()) {
    return it->second;
  }
  throw ExceptionThrower::InvalidConfigException(NOT_SUPPORT_CONFIG);
}
