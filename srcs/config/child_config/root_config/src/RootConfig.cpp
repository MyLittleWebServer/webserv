#include "../include/RootConfig.hpp"

const std::string RootConfig::DEFAULT_USER = "nobody";
const std::string RootConfig::DEFAULT_WORKER_PROCESSES = "1";

RootConfig::RootConfig() {
  _data.insert(std::pair<std::string, std::string>("user", DEFAULT_USER));
  _data.insert(std::pair<std::string, std::string>("worker_processes",
                                                   DEFAULT_WORKER_PROCESSES));
}

RootConfig::~RootConfig() {}

RootConfig::RootConfig(const RootConfig& src) { *this = src; }

RootConfig& RootConfig::operator=(const RootConfig& src) {
  if (this != &src) {
    _data = src._data;
  }
  return *this;
}

void RootConfig::setVariable(const std::string& key, const std::string& value) {
  if (key == "user" or key == "worker_processes")
    _data.insert(std::pair<std::string, std::string>(key, value));
}

std::string RootConfig::getUser() { return getVariable("user"); }

size_t RootConfig::getWorkerProcesses() {
  return std::atoi(getVariable("worker_processes").c_str());
}

std::string RootConfig::getVariable(const std::string& key) {
  std::map<std::string, std::string>::iterator it = _data.find(key);
  if (it != _data.end()) {
    return it->second;
  }
  throw ExceptionThrower::InvalidConfigException(NOT_SUPPORT_CONFIG);
}
