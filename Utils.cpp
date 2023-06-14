#include <cstdarg>
#include <iostream>

// void exitWithPerror(const char *fileName, const char *lineCount, std::string errorName)
void exitWithPerror(std::string errorName)
{
	std::cout << errorName << std::endl;
	exit(EXIT_FAILURE);
}
