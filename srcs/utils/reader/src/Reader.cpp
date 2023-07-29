#include "../include/Reader.hpp"

Reader::Reader() : _checker(FileChecker::getInstance()) {}

Reader::~Reader(void) {}

Reader::Reader(Reader const& src) : _checker(src._checker) { *this = src; }

Reader& Reader::operator=(Reader const& src) {
  if (this != &src) {
    _checker = src._checker;
  }
  return *this;
}

std::string Reader::read(std::string filename) {
  std::string result = "";
  std::string line;

  std::ifstream ipf(filename);
  _checker.open_check(ipf);
  while (std::getline(ipf, line)) {
    _checker.failCheck(ipf);
    result += line;
    if (!ipf.eof()) result += "\n";
  }
  ipf.close();
  return result;
}

Reader& Reader::getInstance() {
  static Reader instance;
  return instance;
}
