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

void throwWithPerror(const std::string &msg);
short getBoundPort(const struct kevent *_currentEvent);
std::string getCurrentTime();
std::string toLowerString(std::string str);
std::string ft_trim(std::string &str);
std::string ft_trimOWS(std::string &str);

std::vector<std::string> ft_split(const std::string &str, char delim);

template <typename T>
std::string itos(T num);
