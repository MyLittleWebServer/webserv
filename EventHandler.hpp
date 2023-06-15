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

class EventHandler : public Kqueue
{
private:
	std::map<int, std::string> _clients;
	const uintptr_t _serverSocket;
	struct kevent *_currentEvent;
	enum
	{
		E_NONE = 0b00000001,
		E_ERROR = 0b00000010,
		E_READ = 0b00000100,
		E_WRITE = 0b00001000,
	};

public :
	EventHandler(uintptr_t serverSocket);
	EventHandler(const EventHandler& src);
	virtual ~EventHandler(void);
	EventHandler& operator=(EventHandler const& rhs);

	void	errorOccuration();
	void	setCurrentEvent(int i);
	void	branchCondition();
};

#endif