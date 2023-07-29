#pragma once

#include <fstream>
#include <iomanip>
#include <iostream>

#include "../../checker/include/FileChecker.hpp"
#include "IReader.hpp"

/**
 * @brief 파일 읽기 모듈 클래스
 * @author middlefitting
 * @date 2023.06.15
 */
class Reader : public IReader {
 private:
  IFileChecker& _checker;

  Reader();
  virtual ~Reader(void);
  Reader(Reader const& src);
  Reader& operator=(Reader const& src);

 public:
  std::string read(std::string filename);

  static Reader& getInstance();
};
