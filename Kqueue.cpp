#include "Kqueue.hpp"

int Kqueue::_kq = 0;
std::vector<struct kevent> Kqueue::_eventsToAdd = std::vector<struct kevent>(); // 변경 이벤트 저장
struct kevent Kqueue::_eventList[CONCURRENT_EVENTS] = {}; // 이벤트를 받을 배열, 최대 8개의 이벤트 동시 처리

Kqueue::Kqueue(void)
{
	if ((this->_kq = kqueue()) == -1)
		exitWithPerror("kqueue() error\n" + std::string(strerror(errno)));
}

Kqueue::~Kqueue() {}

void Kqueue::addEvent(uintptr_t ident, int16_t filter, uint16_t flags, uint32_t fflags,
					  intptr_t data, void *udata)
{
	struct kevent temp_event;

	EV_SET(&temp_event, ident, filter, flags, fflags, data, udata);
	this->_eventsToAdd.push_back(temp_event);
}

void Kqueue::addEvent(uintptr_t ident)
{
	struct kevent temp_event;

	EV_SET(&temp_event, ident, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
	this->_eventsToAdd.push_back(temp_event);
}

int Kqueue::newEvents()
{
	int new_events = kevent(_kq, &_eventsToAdd[0], _eventsToAdd.size(), _eventList, CONCURRENT_EVENTS, NULL);
	if (new_events == -1)
		exitWithPerror("kevent() error\n" + std::string(strerror(errno)));
	_eventsToAdd.clear(); // clear _eventsToAdd for new changes
	return (new_events);
}

const struct kevent &Kqueue::getEvent(int index) const
{
	return (this->_eventList[index]);
}
