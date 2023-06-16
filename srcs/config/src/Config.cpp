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
    this->_user = src._user;
    this->_port = src._port;
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

std::string Config::getUser() { return this->_user; }

unsigned int Config::getPort() { return this->_port; }

/**
 * @brief Config 객체 초기화 수행
 * @param file_path 설정 파일 경로
 * @author middlefitting
 * @date 2023.06.15
 */
void Config::init(const std::string file_path) {
  (void)file_path;
  std::cout << "Config::init() called" << std::endl;
  std::cout << file_path << std::endl;
  std::string file = _reader.read(file_path);
  std::cout << file << std::endl;
}
