#include "../include/LocationConfig.hpp"

LocationConfig::LocationConfig() {}

LocationConfig::LocationConfig(const std::string& route) {
  _data.insert(std::pair<std::string, std::string>("route", route));
}

LocationConfig::~LocationConfig() {}

LocationConfig::LocationConfig(const LocationConfig& src) { *this = src; }

LocationConfig& LocationConfig::operator=(const LocationConfig& src) {
  if (this != &src) {
    _data = src._data;
  }
  return *this;
}

void LocationConfig::setVariable(const std::string& key,
                                 const std::string& value) {
  _data.insert(std::pair<std::string, std::string>(key, value));
}
