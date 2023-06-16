#include "../include/FileChecker.hpp"

FileChecker::FileChecker(void) {}

FileChecker::~FileChecker(void) {}

FileChecker::FileChecker(FileChecker const& src) { *this = src; }

FileChecker& FileChecker::operator=(FileChecker const& src) {
  if (this != &src) {
    *this = src;
  }
  return *this;
}

void FileChecker::open_check(std::ifstream& file) const {
  if (!file.is_open()) {
    throw ExceptionThrower::FileOpenFailedException();
  }
}

void FileChecker::open_check(std::ofstream& file) const {
  if (!file.is_open()) {
    throw ExceptionThrower::FileOpenFailedException();
  }
}

void FileChecker::failCheck(std::ifstream& file) const {
  if (file.fail()) {
    throw ExceptionThrower::FileStatusFailedException();
  }
}

void FileChecker::failCheck(std::ofstream& file) const {
  if (file.fail()) {
    throw ExceptionThrower::FileStatusFailedException();
  }
}

FileChecker& FileChecker::getInstance() {
  static FileChecker instance;
  return instance;
}
