#include "../include/ServerConfig.hpp"

const std::string ServerConfig::SERVER_NAME = "webserv";
const std::string ServerConfig::LISTEN = "8080";
const std::string ServerConfig::ERROR_PAGE =
    "config/error_page/defaultError.html";
const std::string ServerConfig::ACCESS_LOG = "logs/domain1.access.log  main";
const std::string ServerConfig::ROOT = "html";
const std::string ServerConfig::CGI = ".bla";

ServerConfig::ServerConfig() {
  _data.insert(std::pair<std::string, std::string>("server_name", SERVER_NAME));
  _data.insert(std::pair<std::string, std::string>("listen", LISTEN));
  _data.insert(std::pair<std::string, std::string>("error_page", ERROR_PAGE));
  _data.insert(std::pair<std::string, std::string>("access_log", ACCESS_LOG));
  _data.insert(std::pair<std::string, std::string>("root", ROOT));
  _data.insert(std::pair<std::string, std::string>("cgi", CGI));
}

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

const std::string& ServerConfig::getVariable(const std::string& key) {
  std::map<std::string, std::string>::iterator it = _data.find(key);
  if (it != _data.end()) {
    return it->second;
  }
  throw ExceptionThrower::InvalidConfigException(NOT_SUPPORT_CONFIG);
}

std::list<ILocationConfig*> ServerConfig::getLocationConfigs() {
  return _location_configs;
}

size_t ServerConfig::getListen() {
  return std::atoi(getVariable("LISTEN").c_str());
}

const std::string& ServerConfig::getServerName() {
  return getVariable("SERVER_NAME");
}

const std::string& ServerConfig::getErrorPage() {
  return getVariable("ERROR_PAGE");
}

const std::string& ServerConfig::getAccessLog() {
  return getVariable("ACCESS_LOG");
}

const std::string& ServerConfig::getRoot() { return getVariable("ROOT"); }

const std::string& ServerConfig::getCgi() { return getVariable("CGI"); }
