#pragma once

#include <fstream>
#include <iomanip>
#include <iostream>

#include "../../../exception/include/ExceptionThrower.hpp"
#include "IFileChecker.hpp"

/**
 * @brief 파일 체크 모듈 클래스
 * @author middlefitting
 * @date 2023.06.15
 */
class FileChecker : public IFileChecker {
 private:
  FileChecker(void);
  ~FileChecker(void);
  FileChecker(FileChecker const& src);
  FileChecker& operator=(FileChecker const& src);

 public:
  void open_check(std::ifstream& file) const;
  void open_check(std::ofstream& file) const;
  void failCheck(std::ifstream& file) const;
  void failCheck(std::ofstream& file) const;

  static FileChecker& getInstance();
};
