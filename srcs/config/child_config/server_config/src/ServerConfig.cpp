#include "../include/ServerConfig.hpp"

ServerConfig::ServerConfig() {}

ServerConfig::~ServerConfig() {
  for (std::list<ILocationConfig*>::iterator it = _location_configs.begin();
       it != _location_configs.end(); ++it) {
    LocationConfig* tmp = dynamic_cast<LocationConfig*>(*it);
    delete tmp;
  }
}

ServerConfig::ServerConfig(const ServerConfig& src) { *this = src; }

ServerConfig& ServerConfig::operator=(const ServerConfig& src) {
  if (this != &src) {
    _data = src._data;
    for (std::list<ILocationConfig*>::const_iterator it =
             src._location_configs.begin();
         it != src._location_configs.end(); ++it) {
      LocationConfig* tmp = dynamic_cast<LocationConfig*>(*it);
      _location_configs.push_back(new LocationConfig(*tmp));
    }
  }
  return *this;
}

void ServerConfig::setVariable(const std::string& key,
                               const std::string& value) {
  _data.insert(std::pair<std::string, std::string>(key, value));
}

void ServerConfig::addLocationConfig(ILocationConfig* location_config) {
  _location_configs.push_back(location_config);
}
