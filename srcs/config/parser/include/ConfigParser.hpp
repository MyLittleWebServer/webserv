#pragma once

#include <iostream>

#include "../../../exception/include/ExceptionThrower.hpp"
#include "IConfigParser.hpp"

#define WHITE_SPACE "\t\n\v\f\r "

/**
 * @brief 설정 파일을 파싱하는 인터페이스
 * @author middlefitting
 * @date 2023.06.16
 */
class ConfigParser : public IConfigParser {
 private:
  ConfigParser();
  virtual ~ConfigParser();
  ConfigParser(const ConfigParser& src);
  ConfigParser& operator=(const ConfigParser& src);

  std::string getWord(const std::string& config_data, int& ind,
                      std::string delimeter);

  IRootConfig* parseRoot(IRootConfig* root_config,
                         const std::string& config_data);

  void nposCheck(size_t value);

 public:
  static ConfigParser& getInstance();

  virtual IChildConfig* parse(IChildConfig* child_config,
                              const std::string& config_data,
                              ConfigType config_type);
};
