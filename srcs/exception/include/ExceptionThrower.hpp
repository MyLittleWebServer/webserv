#pragma once

#include <iostream>
#include <string>

#include "errorMessage.hpp"

class ExceptionThrower {
 private:
  ExceptionThrower();
  virtual ~ExceptionThrower();
  ExceptionThrower(ExceptionThrower const& src);
  ExceptionThrower& operator=(ExceptionThrower const& src);

 public:
  class BadInputException : public std::invalid_argument {
   public:
    BadInputException(std::string message);

    virtual const char* what() const throw();
  };

  class FileOpenFailedException : public std::runtime_error {
   public:
    FileOpenFailedException();

    virtual const char* what() const throw();
  };

  class FileStatusFailedException : public std::runtime_error {
   public:
    FileStatusFailedException();

    virtual const char* what() const throw();
  };

  class NposException : public std::runtime_error {
   public:
    NposException();

    virtual const char* what() const throw();
  };

  class InvalidConfigException : public std::invalid_argument {
   public:
    InvalidConfigException(std::string message);

    virtual const char* what() const throw();
  };

  class CGINotFinishedException : public std::runtime_error {
   public:
    CGINotFinishedException();

    virtual const char* what() const throw();
  };

  class CGIPipeException : public std::runtime_error {
   public:
    CGIPipeException();

    virtual const char* what() const throw();
  };
};
