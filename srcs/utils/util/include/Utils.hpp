/**
 * @file Utils.hpp
 * @author chanhihi
 * @brief 유틸리티 함수들을 모아놓은 헤더파일입니다.
 * @version 0.1
 * @date 2023-07-20
 *
 * @copyright Copyright (c) 2023
 */
#pragma once
#include <netinet/in.h>
#include <sys/event.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>

#include <cstdarg>
#include <ctime>
#include <iostream>
#include <sstream>

#include "Kqueue.hpp"
#include "Utils.tpp"
#include "vector"

void throwWithErrorMessage(const std::string &message);
short getBoundPort(const struct kevent *_currentEvent);
std::string getCurrentTime();
std::string toLowerString(std::string str);
std::string ft_trim(std::string &str);
std::string ft_trimOWS(std::string &str);

std::vector<std::string> ft_split(const std::string &str, char delim);

template <typename T>
std::string itos(T num);
