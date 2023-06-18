#include "../include/ChildConfigFactory.hpp"

ChildConfigFactory::ChildConfigFactory() {}
ChildConfigFactory::~ChildConfigFactory() {}
ChildConfigFactory::ChildConfigFactory(const ChildConfigFactory& src) {
  *this = src;
}

ChildConfigFactory& ChildConfigFactory::operator=(
    const ChildConfigFactory& src) {
  if (this != &src) {
    // do something
  }
  return *this;
}

std::list<IServerConfig*> ChildConfigFactory::generateServers(
    const std::string& config_data) {
  int ind = 0;
  std::string key;
  std::string value;
  std::list<IServerConfig*> server_list;

  while (true) {
    try {
      key = getWord(config_data, ind, WHITE_SPACE);
      if (key == "server") {
        value = getWord(config_data, ind, WHITE_SPACE);
        if (value == "{") server_list.push_back(new ServerConfig());
      } else
        value = getWord(config_data, ind, ";");
    } catch (const ExceptionThrower::NposException& e) {
      return server_list;
    }
  }
}