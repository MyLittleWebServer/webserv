#include "../include/ProxyConfig.hpp"

ProxyConfig::ProxyConfig() {
  _data.insert(std::pair<std::string, unsigned int>("CLIENT_MAX_BODY_SIZE",
                                                    CLIENT_MAX_BODY_SIZE));
  _data.insert(std::pair<std::string, unsigned int>("REQUEST_URI_LIMIT_SIZE",
                                                    REQUEST_URI_LIMIT_SIZE));
  _data.insert(std::pair<std::string, unsigned int>("REQUEST_HEADER_LIMIT_SIZE",
                                                    REQUEST_HEADER_LIMIT_SIZE));
  _client_max_body_size = std::to_string(CLIENT_MAX_BODY_SIZE);
  _request_uri_limit_size = std::to_string(REQUEST_URI_LIMIT_SIZE);
  _request_header_limit_size = std::to_string(REQUEST_HEADER_LIMIT_SIZE);
}

ProxyConfig::~ProxyConfig() {}

ProxyConfig::ProxyConfig(const ProxyConfig& src) { *this = src; }

ProxyConfig& ProxyConfig::operator=(const ProxyConfig& src) {
  if (this != &src) {
    _data = src._data;
    _client_max_body_size = src._client_max_body_size;
    _request_uri_limit_size = src._request_uri_limit_size;
    _request_header_limit_size = src._request_header_limit_size;
  }
  return *this;
}

void ProxyConfig::setVariable(const std::string& key,
                              const std::string& value) {
  std::map<std::string, size_t>::iterator it = _data.find(key);
  if (it != _data.end()) {
    it->second = std::atoi(value.c_str());
  } else {
    _data.insert(std::pair<std::string, size_t>(key, std::atoi(value.c_str())));
  }
  if (key == "CLIENT_MAX_BODY_SIZE")
    _client_max_body_size = _data[key];
  else if (key == "REQUEST_URI_LIMIT_SIZE")
    _request_uri_limit_size = _data[key];
  else if (key == "REQUEST_HEADER_LIMIT_SIZE")
    _request_header_limit_size = _data[key];
}

const std::string& ProxyConfig::getVariable(const std::string& key) {
  if (key == "CLIENT_MAX_BODY_SIZE")
    return _client_max_body_size;
  else if (key == "REQUEST_URI_LIMIT_SIZE")
    return _request_uri_limit_size;
  else if (key == "REQUEST_HEADER_LIMIT_SIZE")
    return _request_header_limit_size;
  throw ExceptionThrower::InvalidConfigException(NOT_SUPPORT_CONFIG);
}

std::size_t ProxyConfig::getClientMaxBodySize() {
  return _data["CLIENT_MAX_BODY_SIZE"];
}

std::size_t ProxyConfig::getRequestUriLimitSize() {
  return _data["REQUEST_URI_LIMIT_SIZE"];
}

std::size_t ProxyConfig::getRequestHeaderLimitSize() {
  return _data["REQUEST_HEADER_LIMIT_SIZE"];
}
