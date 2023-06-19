#ifndef EventHandler_HPP
# define EventHandler_HPP

#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>

#include <iostream>
#include <map>
#include <vector>

#include "Kqueue.hpp"
#include "Utils.hpp"
#include "Client.hpp"

class Client;

class EventHandler : public Kqueue
{
private:
	std::map<int, Client> _clients;
	const uintptr_t _serverSocket;
	struct kevent *_currentEvent;

	void	acceptClient();
	void	registClient(const uintptr_t clientSocket);
public :
	EventHandler(uintptr_t serverSocket);
	EventHandler(const EventHandler& src);
	virtual ~EventHandler();
	EventHandler& operator=(EventHandler const& rhs);

	void	checkStatus();
	void	checkErrorOnSocket();
	void	setCurrentEvent(int i);
	void	branchCondition();
};

#endif