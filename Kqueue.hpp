#ifndef Kqueue_HPP
# define Kqueue_HPP

#include <sys/event.h>
#include <map>
#include <vector>
#include <iostream>

#include "Utils.hpp"

#define CONCURRENT_EVENTS 8

class Kqueue
{

protected :
	static int                         _kq;
	static std::vector<struct kevent>  _eventsToAdd; // 변경 이벤트 저장
	static struct kevent _eventList[CONCURRENT_EVENTS]; // 이벤트를 받을 배열, 최대 8개의 이벤트 동시 처리
public:
	Kqueue(void);
	Kqueue(const Kqueue& src);
	virtual ~Kqueue(void);
	Kqueue& operator=(Kqueue const& rhs);
public:
	void	addEvent(uintptr_t ident, int16_t filter, uint16_t flags, uint32_t fflags,
			intptr_t data, void *udata);
	void	addEvent(uintptr_t ident);
	int		newEvents(void);
public:
	const struct kevent	&getEvent(int index) const;	
};

#endif