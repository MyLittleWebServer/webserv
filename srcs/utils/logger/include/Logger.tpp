#pragma once

#include "Color.hpp"

/*Regular Message*/
template <typename T>
void Logger::regularCout(T msg) {
  std::cout << msg << std::endl;
}

template <typename T>
void Logger::regularCoutNoEndl(T msg) {
  std::cout << msg;
}

/*Warning Message*/
template <typename T>
void Logger::warningCout(T msg) {
  std::cout << YELLOW << brackets(getTime()) << " [WARNING] " << msg << RESET
            << std::endl;
}

template <typename T>
void Logger::warningCoutNoEndl(T msg) {
  std::cout << YELLOW << brackets(getTime()) << " [WARNING] " << msg << RESET;
}

template <typename T>
void Logger::warningCoutOnlyMsg(T msg) {
  std::cout << YELLOW << msg << RESET;
}

template <typename T>
void Logger::warningCoutOnlyMsgWithEndl(T msg) {
  std::cout << YELLOW << msg << RESET << std::endl;
}

/*Info Message*/
template <typename T>
void Logger::responseCout(T msg) {
  std::cout << BLUE << brackets(getTime()) << " [RESPONSE] " << msg << RESET
            << std::endl;
}

template <typename T>
void Logger::responseCoutNoEndl(T msg) {
  std::cout << BLUE << brackets(getTime()) << " [RESPONSE] " << msg << RESET;
}

template <typename T>
void Logger::responseCoutOnlyMsg(T msg) {
  std::cout << BLUE << msg << RESET;
}

template <typename T>
void Logger::responseCoutOnlyMsgWithEndl(T msg) {
  std::cout << BLUE << msg << RESET << std::endl;
}

/*Info Message*/
template <typename T>
void Logger::connectCout(T msg) {
  std::cout << CYAN << brackets(getTime()) << " [CONNECT] " << msg << RESET
            << std::endl;
}

template <typename T>
void Logger::connectCoutNoEndl(T msg) {
  std::cout << CYAN << brackets(getTime()) << " [CONNECT] " << msg << RESET;
}

template <typename T>
void Logger::connectCoutOnlyMsg(T msg) {
  std::cout << CYAN << msg << RESET;
}

template <typename T>
void Logger::connectCoutOnlyMsgWithEndl(T msg) {
  std::cout << CYAN << msg << RESET << std::endl;
}

/*Info Message*/
template <typename T>
void Logger::requestCout(T msg) {
  std::cout << GREEN << brackets(getTime()) << " [REQUEST] " << msg << RESET
            << std::endl;
}

template <typename T>
void Logger::requestCoutNoEndl(T msg) {
  std::cout << GREEN << brackets(getTime()) << " [REQUEST] " << msg << RESET;
}

template <typename T>
void Logger::requestCoutOnlyMsg(T msg) {
  std::cout << GREEN << msg << RESET;
}

template <typename T>
void Logger::requestCoutOnlyMsgWithEndl(T msg) {
  std::cout << GREEN << msg << RESET << std::endl;
}

/*Error Message*/
template <typename T>
void Logger::errorCout(T msg) {
  std::cout << RED << brackets(getTime()) << " [ERROR] " << msg << RESET
            << std::endl;
}

template <typename T>
void Logger::errorCoutNoEndl(T msg) {
  std::cout << RED << brackets(getTime()) << " [ERROR] " << msg << RESET;
}

template <typename T>
void Logger::errorCoutOnlyMsg(T msg) {
  std::cout << RED << msg << RESET;
}

template <typename T>
void Logger::errorCoutOnlyMsgWithEndl(T msg) {
  std::cout << RED << msg << RESET << std::endl;
}
