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

  static const std::string DEFAULT_ROOT;
  static const std::string DEFAULT_ALLOW_METHOD;
  static const std::string DEFAULT_INDEX;
  static const std::string DEFAULT_AUTO_INDEX;
  static const std::string DEFAULT_LIMIT_CLIENT_BODY_SIZE;

 public:
  LocationConfig(const std::string& route);
  virtual ~LocationConfig();
  LocationConfig(const LocationConfig& src);
  LocationConfig& operator=(const LocationConfig& src);

  virtual void setVariable(const std::string& key, const std::string& value);

  std::string getVariable(const std::string& key);

  virtual std::string getRoute();
  virtual std::string getRoot();
  virtual std::map<std::string, bool> getAllowMethod();
  virtual std::string getIndex();
  virtual std::string getAutoindex();
  virtual size_t getLimitClientBodySize();
};
