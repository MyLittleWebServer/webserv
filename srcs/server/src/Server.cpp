#include "Server.hpp"

#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/event.h>
#include <sys/types.h>

#include <cstdint>
#include <string>

#include "Utils.hpp"

Server::Server(void) : _waitListSize(WAITLISTSIZE) {}

Server::Server(int port) : _port(port) {}

Server::~Server(void) {}

void Server::socketInit(void) {
  this->_socket = socket(PF_INET, SOCK_STREAM, 0);
  if (this->_socket == -1) throwWithPerror("socket() error");
}

void Server::addrInit(void) {
  memset(&_addr, 0, sizeof(_addr));
  _addr.sin_family = AF_INET;
  _addr.sin_addr.s_addr = htonl(INADDR_ANY);
  _addr.sin_port = htons(_port);
}

void Server::bindSocketWithAddr(void) {
  if (bind(this->_socket, (struct sockaddr *)&_addr, sizeof(_addr)) == -1)
    throwWithPerror("bind() error");
}

void Server::listenSocket(void) const {
  if (listen(this->_socket, this->_waitListSize) == -1)
    throwWithPerror("listen() error");
}

void Server::asyncSocket(void) const {
  fcntl(this->_socket, F_SETFL, O_NONBLOCK);
}

uintptr_t Server::getSocket(void) const { return (this->_socket); }

void Server::startServer(void) {
  this->socketInit();
  this->addrInit();
  this->bindSocketWithAddr();
  this->listenSocket();
  this->asyncSocket();
}