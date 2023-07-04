#pragma once

#include <map>

#include "IMimeTypesConfig.hpp"

/**
 * @brief MimeTypes 설정 파일 정보를 제공하는 클래스
 * @author middlefitting
 * @date 2023.06.17
 */
class MimeTypesConfig : public IMimeTypesConfig {
 private:
 public:
  std::map<std::string, std::string> _data;
  MimeTypesConfig();
  virtual ~MimeTypesConfig();
  MimeTypesConfig(const MimeTypesConfig& src);
  MimeTypesConfig& operator=(const MimeTypesConfig& src);

  virtual void setVariable(const std::string& key, const std::string& value);

  virtual const std::string& getVariable(const std::string& key);
};
