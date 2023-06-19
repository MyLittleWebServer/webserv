#pragma once

#include <fstream>
#include <iostream>

#include "IChecker.hpp"

/**
 * @brief 파일 체크 모듈 인터페이스
 * @author middlefitting
 * @date 2023.06.15
 */
class IFileChecker : public IChecker {
 public:
  virtual void open_check(std::ifstream& file) const = 0;
  virtual void open_check(std::ofstream& file) const = 0;
  virtual void failCheck(std::ifstream& file) const = 0;
  virtual void failCheck(std::ofstream& file) const = 0;
};
