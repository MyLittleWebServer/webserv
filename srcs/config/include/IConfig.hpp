#pragma once

#include <iostream>

/**
 * @brief 설정 파일 정보를 제공하는 인터페이스
 * @author middlefitting
 * @date 2023.06.15
 */
class IConfig {
 public:
  virtual std::string getUser() = 0;
  virtual unsigned int getPort() = 0;
  // virtual static T& getInstance(std::string file_path) = 0;
};
