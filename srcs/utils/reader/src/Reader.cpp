#include "../include/Reader.hpp"

Reader::Reader(IFileChecker& checker) : checker(checker) {}

Reader::~Reader(void) {}

Reader::Reader(Reader const& src) : checker(src.checker) { *this = src; }

Reader& Reader::operator=(Reader const& src) {
  if (this != &src) {
    this->checker = src.checker;
  }
  return *this;
}

std::string Reader::read(std::string filename) {
  std::string result = "";
  std::string line;

  std::ifstream ipf(filename);
  checker.open_check(ipf);
  while (std::getline(ipf, line)) {
    checker.failCheck(ipf);
    result += line;
    if (!ipf.eof()) result += "\n";
  }
  ipf.close();
  return result;
}
