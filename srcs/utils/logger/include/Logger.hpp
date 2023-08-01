#pragma once

#include <ctime>
#include <iostream>
#include <string>

template <typename T>
inline std::string brackets(const T &x) {
  return "[" + x + "]";
}

class Logger {
 private:
  Logger();
  virtual ~Logger();
  Logger(Logger const &Logger);
  Logger &operator=(Logger const &Logger);

 public:
  template <typename T>
  static void regularCout(T msg);

  template <typename T>
  static void regularCoutNoEndl(T msg);

  template <typename T>
  static void warningCout(T msg);

  template <typename T>
  static void warningCoutNoEndl(T msg);

  template <typename T>
  static void warningCoutOnlyMsg(T msg);

  template <typename T>
  static void warningCoutOnlyMsgWithEndl(T msg);

  template <typename T>
  static void responseCout(T msg);

  template <typename T>
  static void responseCoutNoEndl(T msg);

  template <typename T>
  static void responseCoutOnlyMsg(T msg);

  template <typename T>
  static void responseCoutOnlyMsgWithEndl(T msg);

  template <typename T>
  static void requestCout(T msg);

  template <typename T>
  static void requestCoutNoEndl(T msg);

  template <typename T>
  static void requestCoutOnlyMsg(T msg);

  template <typename T>
  static void requestCoutOnlyMsgWithEndl(T msg);

  template <typename T>
  static void connectCout(T msg);

  template <typename T>
  static void connectCoutNoEndl(T msg);

  template <typename T>
  static void connectCoutOnlyMsg(T msg);

  template <typename T>
  static void connectCoutOnlyMsgWithEndl(T msg);

  template <typename T>
  static void errorCout(T msg);

  template <typename T>
  static void errorCoutNoEndl(T msg);

  template <typename T>
  static void errorCoutOnlyMsg(T msg);

  template <typename T>
  static void errorCoutOnlyMsgWithEndl(T msg);

  static std::string getTime();
};

#include "Logger.tpp"
