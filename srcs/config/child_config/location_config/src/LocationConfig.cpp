#include "../include/LocationConfig.hpp"

const std::string LocationConfig::DEFAULT_ROUTE = "";
const std::string LocationConfig::DEFAULT_ROOT = "/YoupiBanane/";
const std::string LocationConfig::DEFAULT_ALLOW_METHOD = "";
const std::string LocationConfig::DEFAULT_INDEX = "";
const std::string LocationConfig::DEFAULT_AUTO_INDEX = "off";
const std::string LocationConfig::DEFAULT_LIMIT_CLIENT_BODY_SIZE = "1000000";

void LocationConfig::init() {
  _data.insert(std::pair<std::string, std::string>("route", DEFAULT_ROUTE));

  _data.insert(std::pair<std::string, std::string>("root", DEFAULT_ROOT));
  _data.insert(std::pair<std::string, std::string>("allow_method",
                                                   DEFAULT_ALLOW_METHOD));
  _data.insert(std::pair<std::string, std::string>("index", DEFAULT_INDEX));
  _data.insert(
      std::pair<std::string, std::string>("autoindex", DEFAULT_AUTO_INDEX));
  _data.insert(std::pair<std::string, std::string>(
      "limit_client_body_size", DEFAULT_LIMIT_CLIENT_BODY_SIZE));

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
}

LocationConfig::LocationConfig() { init(); }

LocationConfig::LocationConfig(const std::string& route) {
  init();
  _data.insert(std::pair<std::string, std::string>("route", route));
}

LocationConfig::~LocationConfig() {}

LocationConfig::LocationConfig(const LocationConfig& src) { *this = src; }

LocationConfig& LocationConfig::operator=(const LocationConfig& src) {
  if (this != &src) {
    _data = src._data;
    _method_info = src._method_info;
  }
  return *this;
}

void LocationConfig::setVariable(const std::string& key,
                                 const std::string& value) {
  std::map<std::string, std::string>::iterator it = _data.find(key);
  if (it != _data.end()) {
    it->second = value;
  } else {
    _data.insert(std::pair<std::string, std::string>(key, value));
  }
}

const std::string& LocationConfig::getVariable(const std::string& key) {
  std::map<std::string, std::string>::iterator it = _data.find(key);
  if (it != _data.end()) {
    return it->second;
  }
  throw ExceptionThrower::InvalidConfigException(NOT_SUPPORT_CONFIG);
}

const std::string& LocationConfig::getRoute() { return getVariable("route"); }

const std::string& LocationConfig::getRoot() { return getVariable("root"); }

const std::map<std::string, bool>& LocationConfig::getAllowMethod() {
  std::string methods = getVariable("allow_method");
  size_t ind = 0;
  while (ind < methods.size()) {
    size_t next = methods.find(',', ind);
    if (next == std::string::npos) {
      next = methods.size();
    }
    std::string method = methods.substr(ind, next - ind);
    if (_method_info.find(method) != _method_info.end()) {
      _method_info[method] = true;
    }
    ind = next + 1;
  }
  return _method_info;
}

const std::string& LocationConfig::getIndex() { return getVariable("index"); }

const std::string& LocationConfig::getAutoindex() {
  return getVariable("autoindex");
}

size_t LocationConfig::getLimitClientBodySize() {
  return std::stoi(getVariable("limit_client_body_size"));
}
