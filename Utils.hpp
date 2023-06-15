#pragma once

#include <map>
#include <cstdarg>
#include <iostream>
#include <fcntl.h>
#include <unistd.h>

#include "Client.hpp"

void exitWithPerror(const std::string &msg);
void disconnectClient(int client_fd, std::map<int, Client> &clients);