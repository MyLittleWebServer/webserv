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
  std::map<std::string, bool> _method_info;
  LocationConfig();

  static const std::string DEFAULT_ROUTE;
  static const std::string DEFAULT_ROOT;
  static const std::string DEFAULT_ALLOW_METHOD;
  static const std::string DEFAULT_INDEX;
  static const std::string DEFAULT_AUTO_INDEX;
  static const std::string DEFAULT_LIMIT_CLIENT_BODY_SIZE;

  void init();

 public:
  LocationConfig(const std::string& route);
  virtual ~LocationConfig();
  LocationConfig(const LocationConfig& src);
  LocationConfig& operator=(const LocationConfig& src);

  virtual void setVariable(const std::string& key, const std::string& value);

  virtual const std::string& getVariable(const std::string& key);

  virtual const std::string& getRoute();
  virtual const std::string& getRoot();
  virtual const std::map<std::string, bool>& getAllowMethod();
  virtual const std::string& getIndex();
  virtual const std::string& getAutoindex();
  virtual size_t getLimitClientBodySize();
};
