#include "EventHandler.hpp"

EventHandler::EventHandler(uintptr_t serverSocket) : _serverSocket(serverSocket)
{
}

EventHandler::~EventHandler(void) {}

void EventHandler::errorOccuration()
{
	/* check error event return */
	if (_currentEvent->flags & EV_ERROR)
	{
		if (_currentEvent->ident == _serverSocket)
			exitWithPerror("server socket error");
		else
		{
			std::cerr << "client socket error" << std::endl;
			disconnectClient(_currentEvent->ident, _clients);
		}
	}
}

void EventHandler::setCurrentEvent(int i)
{
	this->_currentEvent = &(this->_eventList[i]);
}

void EventHandler::branchCondition()
{
	if (this->_currentEvent->filter == EVFILT_READ)
	{
		if (this->_currentEvent->ident == _serverSocket)
		{
			/* accept new client */
			int client_socket;
			if ((client_socket = accept(_serverSocket, NULL, NULL)) == -1)
				exitWithPerror("accept() error\n" + std::string(strerror(errno)));
			std::cout << "accept new client: " << client_socket << std::endl;
			fcntl(client_socket, F_SETFL, O_NONBLOCK);

			/* add event for client socket - add read && write event */
			addEvent(client_socket, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
			addEvent(client_socket, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
			_clients[client_socket] = "";
		}
		else if (_clients.find(this->_currentEvent->ident) != _clients.end())
		{
			/* read data from client */
			char buf[1024];
			int n = read(this->_currentEvent->ident, buf, sizeof(buf));
			if (n <= 0)
			{
				if (n < 0)
					std::cerr << "client read error!" << std::endl;
				disconnectClient(this->_currentEvent->ident, _clients);
			}
			else
			{
				buf[n] = '\0';
				_clients[this->_currentEvent->ident] += buf;
				std::cout << "received data from " << this->_currentEvent->ident << ": " << _clients[this->_currentEvent->ident] << std::endl;
			}
		}
	}
	else if (this->_currentEvent->filter == EVFILT_WRITE)
	{
		/* send data to client */
		// std::cout << "쏠쑤있어 " << this->_currentEvent->ident << ": " << _clients[this->_currentEvent->ident] << std::endl;
		std::map<int, std::string>::iterator it = _clients.find(this->_currentEvent->ident);
		if (it != _clients.end() && _clients[this->_currentEvent->ident] != "")
		{
			if (write(this->_currentEvent->ident, _clients[this->_currentEvent->ident].c_str(),
					  _clients[this->_currentEvent->ident].size()) == -1)
			{
				std::cerr << "client write error!" << std::endl;
				disconnectClient(this->_currentEvent->ident, _clients);
			}
			else
				_clients[this->_currentEvent->ident].clear();
		}
	}
}
