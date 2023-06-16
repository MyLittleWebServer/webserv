#pragma once

#include <iostream>

#include "../../child_config/root_config/include/RootConfig.hpp"

typedef enum { ROOT, PROXY, MIME_TYPES, SERVER, LOCATION } ConfigType;

/**
 * @brief 설정 파일을 파싱하는 인터페이스
 * @author middlefitting
 * @date 2023.06.16
 */
class IConfigParser {
 public:
  virtual IChildConfig* parse(IChildConfig* child_config,
                              const std::string& config_data,
                              ConfigType config_type) = 0;
};
