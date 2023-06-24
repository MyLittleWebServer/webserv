#pragma once

#include <fcntl.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstdarg>
#include <iostream>
#include <map>

#include "Client.hpp"

void exitWithPerror(const std::string &msg);
void disconnectClient(int client_fd, std::map<int, Client> &clients);
short getBoundPort(const struct kevent *_currentEvent);
std::string getCurrentTime();
std::string itos(int num);