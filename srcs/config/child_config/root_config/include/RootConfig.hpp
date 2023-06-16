#pragma once

#include "IRootConfig.hpp"

/**
 * @brief Root 설정 파일 정보를 제공하는 클래스
 * @author middlefitting
 * @date 2023.06.16
 */
class RootConfig : public IRootConfig {
 private:
  std::string _user;
  std::string _worker_processes;

 public:
  RootConfig();
  virtual ~RootConfig();
  RootConfig(const RootConfig& src);
  RootConfig& operator=(const RootConfig& src);

  virtual void setVariable(const std::string& key, const std::string& value);

  std::string getUser() const;
  std::string getWorkerProcesses() const;
};
