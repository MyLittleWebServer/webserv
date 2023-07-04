#pragma once

// #include "Client.hpp"

void throwWithPerror(const std::string &msg);
short getBoundPort(const struct kevent *_currentEvent);
std::string getCurrentTime();
std::string itos(int num);
std::string toLowerString(std::string str);