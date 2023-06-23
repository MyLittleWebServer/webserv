#pragma once

#include "IRootConfig.hpp"
#include "map"

/**
 * @brief Root 설정 파일 정보를 제공하는 클래스
 * @author middlefitting
 * @date 2023.06.16
 */
class RootConfig : public IRootConfig {
 private:
  std::map<std::string, std::string> _data;

  static const std::string DEFAULT_USER;
  static const std::string DEFAULT_WORKER_PROCESSES;

 public:
  RootConfig();
  virtual ~RootConfig();
  RootConfig(const RootConfig& src);
  RootConfig& operator=(const RootConfig& src);

  virtual void setVariable(const std::string& key, const std::string& value);

  const std::string& getUser();
  size_t getWorkerProcesses();

  const std::string& getVariable(const std::string& key);
};
