#include "../include/LocationConfig.hpp"

const std::string LocationConfig::DEFAULT_ROOT = "/YoupiBanane/";
const std::string LocationConfig::DEFAULT_ALLOW_METHOD = "";
const std::string LocationConfig::DEFAULT_INDEX = "";
const std::string LocationConfig::DEFAULT_AUTO_INDEX = "off";
const std::string LocationConfig::DEFAULT_LIMIT_CLIENT_BODY_SIZE = "1000000";

LocationConfig::LocationConfig() {
  _data.insert(std::pair<std::string, std::string>("root", DEFAULT_ROOT));
  _data.insert(std::pair<std::string, std::string>("allow_method",
                                                   DEFAULT_ALLOW_METHOD));
  _data.insert(std::pair<std::string, std::string>("index", DEFAULT_INDEX));
  _data.insert(
      std::pair<std::string, std::string>("autoindex", DEFAULT_AUTO_INDEX));
  _data.insert(std::pair<std::string, std::string>(
      "limit_client_body_size", DEFAULT_LIMIT_CLIENT_BODY_SIZE));
}

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

std::string LocationConfig::getVariable(const std::string& key) {
  std::map<std::string, std::string>::iterator it = _data.find(key);
  if (it != _data.end()) {
    return it->second;
  }
  throw ExceptionThrower::InvalidConfigException(NOT_SUPPORT_CONFIG);
}

std::string LocationConfig::getRoute() { return getVariable("route"); }

std::string LocationConfig::getRoot() { return getVariable("root"); }

std::map<std::string, bool> LocationConfig::getAllowMethod() {
  std::string methods = getVariable("allow_method");

  std::map<std::string, bool> result;
  result.insert(std::pair<std::string, bool>("GET", false));
  result.insert(std::pair<std::string, bool>("HEAD", false));
  result.insert(std::pair<std::string, bool>("POST", false));
  result.insert(std::pair<std::string, bool>("PUT", false));
  result.insert(std::pair<std::string, bool>("DELETE", false));
  result.insert(std::pair<std::string, bool>("CONNECT", false));
  result.insert(std::pair<std::string, bool>("OPTIONS", false));
  result.insert(std::pair<std::string, bool>("TRACE", false));
  result.insert(std::pair<std::string, bool>("PATCH", false));

  size_t ind = 0;
  while (ind < methods.size()) {
    size_t next = methods.find(',', ind);
    if (next == std::string::npos) {
      next = methods.size();
    }
    std::string method = methods.substr(ind, next - ind);
    if (result.find(method) != result.end()) {
      result[method] = true;
    }
    ind = next + 1;
  }
  return result;
}

std::string LocationConfig::getIndex() { return getVariable("index"); }

std::string LocationConfig::getAutoindex() { return getVariable("autoindex"); }

size_t LocationConfig::getLimitClientBodySize() {
  return std::stoi(getVariable("limit_client_body_size"));
}
