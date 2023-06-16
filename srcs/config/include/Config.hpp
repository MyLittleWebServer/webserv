#pragma once

#include "../../utils/reader/include/Reader.hpp"
#include "IConfig.hpp"

/**
 * @brief 설정 파일 내용을 관리하는 정보성 클래스
 *
 * Config 정보 구성은 다음 계층을 따른다.
 * Config
 *   RootConfig
 *   ProxyConfig
 *   MimeTypesConfig
 *   HttpConfig
 *     ServerConfig[]
 *       LocationConfig[]
 *
 * @author middlefitting
 * @date 2023.06.16
 */

class Config : public IConfig {
 private:
  static std::string DEFAULT_PATH;
  std::string _user;
  unsigned int _port;
  IReader& _reader;

  Config();
  Config(const std::string path);
  virtual ~Config();
  Config(const Config& src);
  Config& operator=(const Config& src);
  void init(const std::string file_path);

 public:
  virtual std::string getUser();
  virtual unsigned int getPort();
  static Config& getInstance(const std::string file_path);

  static Config& getInstance();
};
