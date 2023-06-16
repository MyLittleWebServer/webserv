#include "../include/ConfigParser.hpp"

ConfigParser::ConfigParser() {}

ConfigParser::~ConfigParser() {}

ConfigParser::ConfigParser(const ConfigParser& src) { *this = src; }

ConfigParser& ConfigParser::operator=(const ConfigParser& src) {
  if (this != &src) {
    // do something
  }
  return *this;
}

ConfigParser& ConfigParser::getInstance() {
  static ConfigParser instance;
  return instance;
}

// { ROOT, PROXY, MIME_TYPES, SERVER, LOCATION }
IChildConfig* ConfigParser::parse(IChildConfig* child_config,
                                  const std::string& config_data,
                                  ConfigType config_type) {
  switch (config_type) {
    case ROOT:
      return parseRoot((IRootConfig*)child_config, config_data);
      break;
    default:
      return child_config;
  }
}

IRootConfig* ConfigParser::parseRoot(IRootConfig* root_config,
                                     const std::string& config_data) {
  int ind = 0;
  std::string key;
  std::string value;

  while (true) {
    try {
      key = getWord(config_data, ind, WHITE_SPACE);
      value = getWord(config_data, ind, ";");
      root_config->setVariable(key, value);
    } catch (const std::exception& e) {
      return root_config;
    }
  }
}

std::string ConfigParser::getWord(const std::string& config_data, int& ind,
                                  std::string delimeter) {
  size_t start = config_data.find_first_not_of(WHITE_SPACE, ind);
  nposCheck(start);
  size_t end = config_data.find_first_of(delimeter, start);
  nposCheck(end);
  std::string result = config_data.substr(start, end - start);
  ind = end + 1;
  return result;
}

void ConfigParser::nposCheck(size_t value) {
  if (value == std::string::npos) throw ExceptionThrower::NposException();
}
