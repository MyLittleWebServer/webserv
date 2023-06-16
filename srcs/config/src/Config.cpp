#include "../include/Config.hpp"

std::string Config::DEFAULT_PATH = "config/mginx.conf";

Config::Config() : _reader(Reader::getInstance()) { init(DEFAULT_PATH); }

Config::Config(const std::string file_path) : _reader(Reader::getInstance()) {
  init(file_path);
}

Config::~Config() {}

Config::Config(const Config& src) : _reader(src._reader) { *this = src; }

Config& Config::operator=(const Config& src) {
  if (this != &src) {
    // do something
  }
  return *this;
}

Config& Config::getInstance() {
  static Config instance;
  return instance;
}

Config& Config::getInstance(const std::string file_path) {
  static Config instance(file_path);
  return instance;
}

void Config::init(const std::string file_path) {
  (void)file_path;
  std::cout << "Config::init() called" << std::endl;
  std::cout << file_path << std::endl;
  std::string file = _reader.read(file_path);
  std::cout << file << std::endl;

  IConfigParser& parser = ConfigParser::getInstance();

  RootConfig rootConfig = RootConfig();
  parser.parse(&rootConfig, file, ROOT);
  std::cout << "root_config->getUser(): " << rootConfig.getUser() << std::endl;
  std::cout << "root_config->getWorkerProcesses(): "
            << rootConfig.getWorkerProcesses() << std::endl;
  ProxyConfig proxyConfig = ProxyConfig();

  parser.parse(&proxyConfig, file, PROXY);

  MimeTypesConfig mimeTypesConfig = MimeTypesConfig();
  parser.parse(&mimeTypesConfig, file, MIME_TYPES);
}
