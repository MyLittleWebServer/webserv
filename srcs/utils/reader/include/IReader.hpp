#pragma once
#include <iostream>

/**
 * @brief 파일 읽기 모듈 인터페이스
 * @author middlefitting
 * @date 2023.06.15
 */
class IReader {
 public:
  virtual std::string read(std::string filename) = 0;
};
