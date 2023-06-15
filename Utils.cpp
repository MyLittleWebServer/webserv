#include "Utils.hpp"

// void exitWithPerror(const char *fileName, const char *lineCount, std::string errorName)
void exitWithPerror(const std::string &msg)
{
	std::cerr << msg << std::endl;
	exit(EXIT_FAILURE);
}

void disconnectClient(int client_fd, std::map<int, std::string> &clients)
{
	std::cout << "client disconnected: " << client_fd << std::endl;
	close(client_fd);
	clients.erase(client_fd);
}
