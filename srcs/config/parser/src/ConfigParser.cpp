#include "../include/ConfigParser.hpp"

ConfigParser::ConfigParser() : _reader(Reader::getInstance()) {}

ConfigParser::~ConfigParser() {}

ConfigParser::ConfigParser(const ConfigParser& src) : _reader(src._reader) {
  *this = src;
}

ConfigParser& ConfigParser::operator=(const ConfigParser& src) {
  if (this != &src) {
    _reader = src._reader;
  }
  return *this;
}

ConfigParser& ConfigParser::getInstance() {
  static ConfigParser instance;
  return instance;
}

void ConfigParser::parse(IChildConfig* child_config,
                         const std::string& config_data,
                         ConfigType config_type) {
  switch (config_type) {
    case ROOT:
      parseDefault(child_config, config_data);
      break;
    case PROXY:
      parseInclude(child_config, config_data, "proxy.conf");
      break;
    case MIME_TYPES:
      parseInclude(child_config, config_data, "mime.types");
      break;
    default:
      return;
  }
}

void ConfigParser::parseDefault(IChildConfig* config,
                                const std::string& config_data) {
  int ind = 0;
  std::string key;
  std::string value;

  while (true) {
    try {
      key = getWord(config_data, ind, WHITE_SPACE);
      value = getWord(config_data, ind, ";");
      config->setVariable(key, value);
    } catch (const ExceptionThrower::NposException& e) {
      break;
    }
  }
}

void ConfigParser::parseInclude(IChildConfig* config,
                                const std::string& config_data,
                                std::string file_name) {
  int ind = 0;
  std::string key;
  std::string value;
  size_t f_len = file_name.length();

  while (true) {
    try {
      key = getWord(config_data, ind, WHITE_SPACE);
      value = getWord(config_data, ind, ";");
      if (key == "include" && value.length() >= f_len &&
          value.substr(value.length() - f_len, f_len) == file_name)
        return parseDefault(config, _reader.read(value));
    } catch (const ExceptionThrower::NposException& e) {
      break;
    }
  }
}

std::list<IServerConfig*> ConfigParser::generateServers(
    const std::string& config_data) {
  int ind = 0;
  std::string key;
  std::string value;
  std::list<IServerConfig*> server_list;

  while (true) {
    try {
      key = getWord(config_data, ind, WHITE_SPACE);
      if (key == "server") {
        server_list.push_back(parseAndGenerateServer(ind, config_data));
        continue;
      }
      value = getWord(config_data, ind, ";");
    } catch (const ExceptionThrower::NposException& e) {
      return server_list;
    }
  }
}

IServerConfig* ConfigParser::parseAndGenerateServer(
    int& ind, const std::string& config_data) {
  std::string key = getWord(config_data, ind, WHITE_SPACE);
  std::string value;
  keyCheck(key, "{", INVALID_SERVER_CONFIG);
  IServerConfig* serverConfig = new ServerConfig();

  while (true) {
    try {
      key = getWord(config_data, ind, WHITE_SPACE);
      if (key == "}") return serverConfig;
      if (key == "location") {
        value = getWord(config_data, ind, WHITE_SPACE);
        serverConfig->addLocationConfig(
            parseAndGenerateLocation(ind, config_data, value));
        continue;
      }
      value = getWord(config_data, ind, ";");
      serverConfig->setVariable(key, value);
    } catch (const ExceptionThrower::NposException& e) {
      throw ExceptionThrower::InvalidConfigException(INVALID_SERVER_CONFIG);
    }
  }
  return serverConfig;
}

ILocationConfig* ConfigParser::parseAndGenerateLocation(
    int& ind, const std::string& config_data, std::string route) {
  std::string key = getWord(config_data, ind, WHITE_SPACE);
  std::string value;
  keyCheck(key, "{", INVALID_LOCATION_CONFIG);
  ILocationConfig* locationConfig = new LocationConfig(route);

  while (true) {
    try {
      key = getWord(config_data, ind, WHITE_SPACE);
      if (key == "}") return locationConfig;
      value = getWord(config_data, ind, ";");
      locationConfig->setVariable(key, value);
    } catch (const ExceptionThrower::NposException& e) {
      throw ExceptionThrower::InvalidConfigException(INVALID_LOCATION_CONFIG);
    }
  }
  return locationConfig;
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

void ConfigParser::keyCheck(const std::string& key, const std::string& expect,
                            std::string msg) {
  if (key != expect) throw ExceptionThrower::InvalidConfigException(msg);
}
