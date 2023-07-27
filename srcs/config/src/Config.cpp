#include "../include/Config.hpp"

std::string Config::DEFAULT_PATH = "config/default.conf";

Config::Config()
    : _reader(Reader::getInstance()), _parser(ConfigParser::getInstance()) {
  init(DEFAULT_PATH);
}

Config::Config(const std::string file_path)
    : _reader(Reader::getInstance()), _parser(ConfigParser::getInstance()) {
  init(file_path);
}

Config::~Config() {
  for (std::list<IServerConfig*>::iterator it = _server_configs.begin();
       it != _server_configs.end(); ++it) {
    ServerConfig* tmp = dynamic_cast<ServerConfig*>(*it);
    delete tmp;
  }
}

Config::Config(const Config& src) : _reader(src._reader), _parser(src._parser) {
  *this = src;
}

Config& Config::operator=(const Config& src) {
  if (this != &src) {
    _reader = src._reader;
    _parser = src._parser;
    _root_config = src._root_config;
    _proxy_config = src._proxy_config;
    _mime_types_config = src._mime_types_config;
    for (std::list<IServerConfig*>::const_iterator it =
             src._server_configs.begin();
         it != src._server_configs.end(); ++it) {
      ServerConfig* tmp = dynamic_cast<ServerConfig*>(*it);
      _server_configs.push_back(new ServerConfig(*tmp));
    }
  }
  return *this;
}

Config& Config::getInstance() { return getInstance(DEFAULT_PATH); }

Config& Config::getInstance(const std::string file_path) {
  static Config instance(file_path);
  return instance;
}

void Config::init(const std::string file_path) {
  std::string file = _reader.read(file_path);
  _parser.parse(&_root_config, file, ROOT);
  _parser.parse(&_proxy_config, file, PROXY);
  _parser.parse(&_mime_types_config, file, MIME_TYPES);
  _server_configs = _parser.generateServers(file);
}

IRootConfig& Config::getRootConfig() { return _root_config; }

IProxyConfig& Config::getProxyConfig() { return _proxy_config; }

IMimeTypesConfig& Config::getMimeTypesConfig() { return _mime_types_config; }

std::list<IServerConfig*>& Config::getServerConfigs() {
  return _server_configs;
}
