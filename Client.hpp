#ifndef Client_HPP
# define Client_HPP

#include <iostream>
#include <sys/socket.h>
#include <fcntl.h>
#include <unistd.h>
#include <map>
#include <string>

#define RECEIVE_LEN 10

class Client
{
private:
	const uintptr_t	_sd;
	std::string	_request;
	std::string	_response;
public:
	Client(void);
	Client(const uintptr_t sd);
	virtual ~Client(void);
public:
	void	receiveData(void);
	void 	sendData(std::map<int, Client> &_clients);
};

#endif
