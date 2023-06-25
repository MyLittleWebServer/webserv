#include "Kqueue.hpp"

int Kqueue::_kq = 0;
std::vector<struct kevent> Kqueue::_eventsToAdd = std::vector<struct kevent>();
struct kevent Kqueue::_eventList[CONCURRENT_EVENTS] = {};

Kqueue::Kqueue(void) {
  if ((this->_kq = kqueue()) == -1)
    throwWithPerror("kqueue() error\n" + std::string(strerror(errno)));
}

Kqueue::~Kqueue() {}

void Kqueue::addEvent(uintptr_t ident, int16_t filter, uint16_t flags,
                      uint32_t fflags, intptr_t data, void *udata) {
  struct kevent temp_event;

  std::cout << "addEvent: " << ident << filter << flags << fflags << data
            << udata << std::endl;

  EV_SET(&temp_event, ident, filter, flags, fflags, data, udata);
  this->_eventsToAdd.push_back(temp_event);
}

void Kqueue::addEvent(uintptr_t ident) {
  struct kevent temp_event;

  EV_SET(&temp_event, ident, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
  this->_eventsToAdd.push_back(temp_event);
}

int Kqueue::newEvents() {
  int new_events = kevent(_kq, &_eventsToAdd[0], _eventsToAdd.size(),
                          _eventList, CONCURRENT_EVENTS, NULL);
  if (new_events == -1)
    throwWithPerror("kevent() error\n" + std::string(strerror(errno)));
  _eventsToAdd.clear();
  return (new_events);
}

const struct kevent &Kqueue::getEvent(int index) const {
  return (this->_eventList[index]);
}
