#include "Logger.hpp"

Logger::Logger() {
  std::string msg = "Logger default constructor called";
  regularCout(msg);
}

Logger::~Logger() {
  std::string msg = "Logger destructor called";
  regularCout(msg);
}

Logger::Logger(Logger const &cout_util) {
  std::string msg = "Logger copy constructor called";
  regularCout(msg);
  *this = cout_util;
}

Logger &Logger::operator=(Logger const &cout_util) {
  std::string msg = "Logger copy assignment operator called";
  regularCout(msg);
  (void)cout_util;
  return *this;
}

std::string Logger::getTime() {
  std::time_t now = std::time(nullptr);
  char buffer[20];
  const char *format = "%Y-%m-%d %H:%M:%S";
  std::strftime(buffer, sizeof(buffer), format, std::localtime(&now));
  return buffer;
}
