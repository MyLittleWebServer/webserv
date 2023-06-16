#pragma once

#include <iostream>

#include "../../../exception/include/ExceptionThrower.hpp"
#include "../../../utils/reader/include/Reader.hpp"
#include "../../child_config/mime_types_config/include/MimeTypesConfig.hpp"
#include "../../child_config/proxy_config/include/ProxyConfig.hpp"
#include "../../child_config/root_config/include/RootConfig.hpp"
#include "IConfigParser.hpp"

#define WHITE_SPACE "\t\n\v\f\r "

/**
 * @brief 설정 파일을 파싱하는 인터페이스
 * @author middlefitting
 * @date 2023.06.16
 */
class ConfigParser : public IConfigParser {
 private:
  IReader& _reader;

  ConfigParser();
  virtual ~ConfigParser();
  ConfigParser(const ConfigParser& src);
  ConfigParser& operator=(const ConfigParser& src);

  std::string getWord(const std::string& config_data, int& ind,
                      std::string delimeter);

  void defaultParse(IChildConfig* config, const std::string& config_data);
  void parseInclude(IChildConfig* config, const std::string& config_data,
                    std::string file_name);
  void nposCheck(size_t value);

 public:
  static ConfigParser& getInstance();

  virtual void parse(IChildConfig* child_config, const std::string& config_data,
                     ConfigType config_type);
};
