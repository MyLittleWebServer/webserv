#pragma once

#include <fcntl.h>
#include <unistd.h>

#include <cstdarg>
#include <iostream>
#include <map>

#include "Client.hpp"

void throwWithPerror(const std::string &msg);
void disconnectClient(int client_fd, std::map<int, Client> &clients);
short getBoundPort(const struct kevent *_currentEvent);
std::string getCurrentTime();
std::string itos(int num);
std::string toLowerString(std::string str);