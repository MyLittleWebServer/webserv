#pragma once

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
  std::cout << "\033[33m"
            << "[" << getTime() << "]"
            << " [WARNING] " << msg << "\033[0m" << std::endl;
}

template <typename T>
void Logger::warningCoutNoEndl(T msg) {
  std::cout << "\033[33m"
            << "[" << getTime() << "]"
            << " [WARNING] " << msg << "\033[0m";
}

template <typename T>
void Logger::warningCoutOnlyMsg(T msg) {
  std::cout << "\033[33m" << msg << "\033[0m";
}

template <typename T>
void Logger::warningCoutOnlyMsgWithEndl(T msg) {
  std::cout << "\033[33m" << msg << "\033[0m" << std::endl;
}

/*Info Message*/
template <typename T>
void Logger::responseCout(T msg) {
  std::cout << "\033[34m"
            << "[" << getTime() << "]"
            << " [RESPONSE] " << msg << "\033[0m" << std::endl;
}

template <typename T>
void Logger::responseCoutNoEndl(T msg) {
  std::cout << "\033[34m"
            << "[" << getTime() << "]"
            << " [RESPONSE] " << msg << "\033[0m";
}

template <typename T>
void Logger::responseCoutOnlyMsg(T msg) {
  std::cout << "\033[34m" << msg << "\033[0m";
}

template <typename T>
void Logger::responseCoutOnlyMsgWithEndl(T msg) {
  std::cout << "\033[34m" << msg << "\033[0m" << std::endl;
}

/*Info Message*/
template <typename T>
void Logger::connectCout(T msg) {
  std::cout << "\033[36m"
            << "[" << getTime() << "]"
            << " [CONNECT] " << msg << "\033[0m" << std::endl;
}

template <typename T>
void Logger::connectCoutNoEndl(T msg) {
  std::cout << "\033[36m"
            << "[" << getTime() << "]"
            << " [CONNECT] " << msg << "\033[0m";
}

template <typename T>
void Logger::connectCoutOnlyMsg(T msg) {
  std::cout << "\033[36m" << msg << "\033[0m";
}

template <typename T>
void Logger::connectCoutOnlyMsgWithEndl(T msg) {
  std::cout << "\033[36m" << msg << "\033[0m" << std::endl;
}

/*Info Message*/
template <typename T>
void Logger::requestCout(T msg) {
  std::cout << "\033[32m"
            << "[" << getTime() << "]"
            << " [REQUEST] " << msg << "\033[0m" << std::endl;
}

template <typename T>
void Logger::requestCoutNoEndl(T msg) {
  std::cout << "\033[32m"
            << "[" << getTime() << "]"
            << " [REQUEST] " << msg << "\033[0m";
}

template <typename T>
void Logger::requestCoutOnlyMsg(T msg) {
  std::cout << "\033[32m" << msg << "\033[0m";
}

template <typename T>
void Logger::requestCoutOnlyMsgWithEndl(T msg) {
  std::cout << "\033[32m" << msg << "\033[0m" << std::endl;
}

/*Error Message*/
template <typename T>
void Logger::errorCout(T msg) {
  std::cout << "\033[31m"
            << "[" << getTime() << "]"
            << " [ERROR] " << msg << "\033[0m" << std::endl;
}

template <typename T>
void Logger::errorCoutNoEndl(T msg) {
  std::cout << "\033[31m"
            << "[" << getTime() << "]"
            << " [ERROR] " << msg << "\033[0m";
}

template <typename T>
void Logger::errorCoutOnlyMsg(T msg) {
  std::cout << "\033[31m" << msg << "\033[0m";
}

template <typename T>
void Logger::errorCoutOnlyMsgWithEndl(T msg) {
  std::cout << "\033[31m" << msg << "\033[0m" << std::endl;
}
