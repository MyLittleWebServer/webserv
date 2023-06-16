#include "../include/RootConfig.hpp"

RootConfig::RootConfig() {}

RootConfig::~RootConfig() {}

RootConfig::RootConfig(const RootConfig& src) { *this = src; }

RootConfig& RootConfig::operator=(const RootConfig& src) {
  if (this != &src) {
    // do something
  }
  return *this;
}

void RootConfig::setVariable(const std::string& key, const std::string& value) {
  if (key == "user")
    _user = value;
  else if (key == "worker_processes")
    _worker_processes = value;
}

std::string RootConfig::getUser() const { return _user; }

std::string RootConfig::getWorkerProcesses() const { return _worker_processes; }
