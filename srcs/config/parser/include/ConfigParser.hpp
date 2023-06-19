#pragma once

#include <iostream>
#include <list>

#include "../../../exception/include/ExceptionThrower.hpp"
#include "../../../utils/reader/include/Reader.hpp"
#include "../../child_config/location_config/include/LocationConfig.hpp"
#include "../../child_config/mime_types_config/include/MimeTypesConfig.hpp"
#include "../../child_config/proxy_config/include/ProxyConfig.hpp"
#include "../../child_config/root_config/include/RootConfig.hpp"
#include "../../child_config/server_config/include/ServerConfig.hpp"
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

  void parseDefault(IChildConfig* config, const std::string& config_data);
  void parseInclude(IChildConfig* config, const std::string& config_data,
                    std::string file_name);

  IServerConfig* parseAndGenerateServer(int& ind,
                                        const std::string& config_data);

  ILocationConfig* parseAndGenerateLocation(int& ind,
                                            const std::string& config_data,
                                            std::string route);

  void nposCheck(size_t value);
  std::string getWord(const std::string& config_data, int& ind,
                      std::string delimeter);

  void keyCheck(const std::string& key, const std::string& expect,
                std::string msg);

 public:
  static ConfigParser& getInstance();

  virtual void parse(IChildConfig* child_config, const std::string& config_data,
                     ConfigType config_type);
  std::list<IServerConfig*> generateServers(const std::string& config_data);
};
