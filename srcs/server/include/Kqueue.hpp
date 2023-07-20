#ifndef Kqueue_HPP
#define Kqueue_HPP

#include <sys/event.h>

#include <iostream>
#include <map>
#include <vector>

#include "Utils.hpp"

#define CONCURRENT_EVENTS 1024

typedef enum {
  FD_NONE,
  FD_SERVER,
  FD_CLIENT,
  FD_METHOD,
  FD_CGI,
} e_fd_type;

class Kqueue {
 private:
  static fd_set _server_fds;
  static fd_set _client_fds;
  static fd_set _method_fds;
  static fd_set _cgi_fds;

 protected:
  static int _kq;
  static std::vector<struct kevent> _eventsToAdd;
  static struct kevent _eventList[CONCURRENT_EVENTS];

 public:
  Kqueue(void);
  virtual ~Kqueue(void);

 public:
  static void init(void);
  static void addEvent(uintptr_t ident, int16_t filter, uint16_t flags,
                       uint32_t fflags, intptr_t data, void* udata);
  static void addEvent(uintptr_t ident);

  static void registEvent(uintptr_t ident, int16_t filter, uint16_t flags,
                          uint32_t fflags, intptr_t data, void* udata);

  static void enableEvent(uintptr_t ident, int16_t filter, void* udata);
  static void disableEvent(uintptr_t ident, int16_t filter, void* udata);
  static void deleteEvent(uintptr_t ident, int16_t filter, void* udata);

  int newEvents(void);

  static e_fd_type getFdType(uintptr_t ident);
  static void setFdSet(uintptr_t ident, e_fd_type type);
  static void deleteFdSet(uintptr_t ident, e_fd_type type);

 public:
  const struct kevent& getEvent(int index) const;
};

#endif
