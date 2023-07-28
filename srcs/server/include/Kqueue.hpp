#ifndef Kqueue_HPP
#define Kqueue_HPP

#include <sys/event.h>

#include <iostream>
#include <limits>
#include <map>
#include <vector>

#include "Utils.hpp"

#define CONCURRENT_EVENTS 1024

/**
 * @brief fd type
 * @details
 * FD_NONE : fd가 아님
 * FD_SERVER : 서버의 fd
 * FD_CLIENT : 클라이언트의 fd
 * FD_METHOD : 메소드의 fd
 * FD_CGI : CGI의 fd
 */
typedef enum {
  FD_NONE,
  FD_SERVER,
  FD_CLIENT,
  FD_METHOD,
  FD_CGI,
} e_fd_type;

typedef enum { SESSION_TIMER = 2147483647 } e_timer_type;

/**
 * @brief Kqueue class : kevent와 FD를 관리하는 클래스입니다.
 *
 * @details
 * Kqueue는 kevent와 FD를 관리하는 클래스입니다.
 * fd_set _server_fds, _client_fds, _method_fds, _cgi_fds를 갖고 있습니다.
 * _server_fds는 서버의 fd_set입니다.
 * _client_fds는 클라이언트의 fd_set입니다.
 * _method_fds는 메소드의 fd_set입니다.
 * _cgi_fds는 CGI의 fd_set입니다.
 * _kq는 kqueue의 fd입니다.
 * _eventsToAdd는 kevent를 추가하기 전에 임시로 저장하는 vector입니다.
 * _eventList는 kevent를 저장하는 배열입니다.
 * CONCURRENT_EVENTS는 kevent를 저장하는 배열의 크기입니다.
 * _server_fds, _client_fds, _method_fds, _cgi_fds는 각각 서버, 클라이언트,
 * 메소드, CGI의 fd_set입니다.
 *
 * @see kevent
 * @see FD
 *
 * @author chanhihi
 * @date 2023.07.17
 */
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
  static void addTimerEvent();

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
