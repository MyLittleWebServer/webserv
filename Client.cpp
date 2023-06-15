#include "Client.hpp"

Client::Client() : _sd(0)
{
}

Client::Client(const uintptr_t sd) : _sd(sd)
{
}

Client::~Client(void)
{
}

void	Client::receiveData(void)
{	
	char buf[RECEIVE_LEN + 1];

	while (1)
	{
		int n = recv(this->_sd, buf, RECEIVE_LEN, 0);
		std::cout << "n: " << n << '\n';
		if (n <= 0)
		{
			if (n < 0)
				throw std::runtime_error("Client read error!");
			throw std::runtime_error("Client disconnected!");
		}
		buf[n] = '\0';
		this->_request += buf;
		std::memset(buf, 0, sizeof(buf));
		if (n < RECEIVE_LEN || recv(this->_sd, buf, RECEIVE_LEN, MSG_PEEK) == -1)
		{
			std::cout << "received data from " << this->_sd << ": " << this->_request << std::endl;
			return ;
		}
	}
}


void	Client::sendData(std::map<int, Client> &_clients)
{
	std::map<int, Client>::iterator it = _clients.find(this->_sd);
	if (it != _clients.end() && this->_request != "")
	{
		if (write(this->_sd, this->_request.c_str(),
					this->_request.size()) == -1)
			throw std::runtime_error("Client write error!");
		else
			_request.clear();
	}
}


