#ifndef Server_HPP
# define Server_HPP

#include <sys/socket.h>
#include <netinet/in.h>

class Server
{
	typedef struct sockaddr_in sAddr_t;
private:
	sAddr_t _addr;
	int		_socket;
	int		_waitListSize;
public:
			Server(void);
	virtual	~Server(void);
public:
	uintptr_t	getSocket(void) const;

public:
	void	    socketInit(void);
	void	    addrInit(void);
	void	    bindSocketWithAddr(void);
	void	    listenSocket(void) const;
	void	    asyncSocket(void) const;
	void		startServer(void);
};


#endif