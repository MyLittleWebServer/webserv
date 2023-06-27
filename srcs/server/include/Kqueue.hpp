#ifndef Kqueue_HPP
#define Kqueue_HPP

#include <sys/event.h>

#include <iostream>
#include <map>
#include <vector>

#include "Utils.hpp"

#define CONCURRENT_EVENTS 1024

class Kqueue {
 protected:
  static int _kq;
  static std::vector<struct kevent> _eventsToAdd;
  static struct kevent _eventList[CONCURRENT_EVENTS];

 public:
  Kqueue(void);
  virtual ~Kqueue(void);

 public:
  void addEvent(uintptr_t ident, int16_t filter, uint16_t flags,
                uint32_t fflags, intptr_t data, void* udata);
  void addEvent(uintptr_t ident);
  static void deleteEvent(uintptr_t ident, int16_t filter);
  int newEvents(void);

 public:
  const struct kevent& getEvent(int index) const;
};

#endif