#include "../include/ExceptionThrower.hpp"

ExceptionThrower::ExceptionThrower() {}

ExceptionThrower::~ExceptionThrower() {}

ExceptionThrower::ExceptionThrower(ExceptionThrower const& src) { (void)src; }

ExceptionThrower& ExceptionThrower::operator=(ExceptionThrower const& src) {
  if (this == &src) return *this;
  delete this;
  new (this) ExceptionThrower(src);
  return *this;
}

ExceptionThrower::BadInputException::BadInputException(std::string message)
    : std::invalid_argument("Error: bad input => " + message) {}

const char* ExceptionThrower::BadInputException::what() const throw() {
  return std::invalid_argument::what();
}

ExceptionThrower::FileOpenFailedException::FileOpenFailedException()
    : std::runtime_error(FILE_OPEN_FAILED) {}

const char* ExceptionThrower::FileOpenFailedException::what() const throw() {
  return std::runtime_error::what();
}

ExceptionThrower::FileStatusFailedException::FileStatusFailedException()
    : std::runtime_error(FILE_STATUS_FAILED) {}

const char* ExceptionThrower::FileStatusFailedException::what() const throw() {
  return std::runtime_error::what();
}

ExceptionThrower::NposException::NposException()
    : std::runtime_error(NPOS_STATUS) {}

const char* ExceptionThrower::NposException::what() const throw() {
  return std::runtime_error::what();
}

ExceptionThrower::InvalidConfigException::InvalidConfigException(
    std::string message)
    : std::invalid_argument(message) {}

const char* ExceptionThrower::InvalidConfigException::what() const throw() {
  return std::invalid_argument::what();
}

ExceptionThrower::CGINotFinishedException::CGINotFinishedException()
    : std::runtime_error(CGI_NOT_FINISHED) {}

const char* ExceptionThrower::CGINotFinishedException::what() const throw() {
  return std::runtime_error::what();
}

ExceptionThrower::CGIPipeException::CGIPipeException()
    : std::runtime_error(CGI_NOT_FINISHED) {}

const char* ExceptionThrower::CGIPipeException::what() const throw() {
  return std::runtime_error::what();
}

ExceptionThrower::FileAccessFailedException::FileAccessFailedException()
    : std::runtime_error(FILE_ACCESS_FAILED) {}

const char* ExceptionThrower::FileAccessFailedException::what() const throw() {
  return std::runtime_error::what();
}

ExceptionThrower::CGIBodyTooLongException::CGIBodyTooLongException()
    : std::runtime_error(CGI_BODY_TOO_LONG) {}

const char* ExceptionThrower::CGIBodyTooLongException::what() const throw() {
  return std::runtime_error::what();
}
ExceptionThrower::SessionDataError::SessionDataError()
    : std::runtime_error(SESSION_DATA_ERROR) {}

const char* ExceptionThrower::SessionDataError::what() const throw() {
  return std::runtime_error::what();
}

ExceptionThrower::SessionDataNotFound::SessionDataNotFound()
    : std::runtime_error(SESSION_DATA_NOT_FOUND) {}

const char* ExceptionThrower::SessionDataNotFound::what() const throw() {
  return std::runtime_error::what();
}

ExceptionThrower::SessionDataExpired::SessionDataExpired()
    : std::runtime_error(SESSION_DATA_ERROR) {}

const char* ExceptionThrower::SessionDataExpired::what() const throw() {
  return std::runtime_error::what();
}
