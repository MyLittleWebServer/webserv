#pragma once

#include <map>

#include "../../include/IChildConfig.hpp"

/**
 * @brief Location 설정 파일 정보를 제공하는 인터페이스
 * @author middlefitting
 * @date 2023.06.17
 */
class ILocationConfig : public IChildConfig {
 public:
  virtual const std::string& getRoute() = 0;
  virtual const std::string& getRoot() = 0;
  virtual const std::map<std::string, bool>& getAllowMethod() = 0;
  virtual const std::string& getIndex() = 0;
  virtual const std::string& getAutoindex() = 0;
  virtual size_t getLimitClientBodySize() = 0;
};
