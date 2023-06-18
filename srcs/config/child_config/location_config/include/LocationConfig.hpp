#pragma once

#include <map>

#include "ILocationConfig.hpp"

/**
 * @brief Location 설정 파일 정보를 제공하는 클래스
 * @author middlefitting
 * @date 2023.06.17
 */
class LocationConfig : public ILocationConfig {
 private:
  std::map<std::string, std::string> _data;
  LocationConfig();

 public:
  LocationConfig(const std::string& route);
  virtual ~LocationConfig();
  LocationConfig(const LocationConfig& src);
  LocationConfig& operator=(const LocationConfig& src);

  virtual void setVariable(const std::string& key, const std::string& value);
};
