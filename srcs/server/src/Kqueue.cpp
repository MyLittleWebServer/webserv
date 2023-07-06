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
                      uint32_t fflags, intptr_t data, void* udata) {
  struct kevent temp_event;

#ifdef DEBUG_MSG
  std::cout << "addEvent: " << ident << filter << flags << fflags << data
            << udata << std::endl;
#endif

  EV_SET(&temp_event, ident, filter, flags, fflags, data, udata);
  Kqueue::_eventsToAdd.push_back(temp_event);
}

void Kqueue::addEvent(uintptr_t ident) {
  struct kevent temp_event;

  EV_SET(&temp_event, ident, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
  Kqueue::_eventsToAdd.push_back(temp_event);
}

void Kqueue::addEvent(uintptr_t ident) {
  struct kevent temp_event;

  EV_SET(&temp_event, ident, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
  Kqueue::_eventsToAdd.push_back(temp_event);
}

void Kqueue::addEvent(uintptr_t ident, int16_t filter, uint16_t flags,
                      uint32_t fflags, intptr_t data, void* udata,
                      e_fd_type type) {
  struct kevent temp_event;
  EV_SET(&temp_event, ident, filter, flags, fflags, data, udata);
  int ret = kevent(Kqueue::_kq, &temp_event, 1, NULL, 0, NULL);
  if (ret == -1)
    throwWithPerror("kevent() error on enableEvent()\n" +
                    std::string(strerror(errno)));
  setFdSet(ident, type);
}

void Kqueue::disableEvent(uintptr_t ident, int16_t filter, void* udata) {
  struct kevent temp_event;

  EV_SET(&temp_event, ident, filter, EV_DISABLE, 0, 0, udata);
  int ret = kevent(Kqueue::_kq, &temp_event, 1, NULL, 0, NULL);
  if (ret == -1)
    throwWithPerror("kevent() error on disableEvent()\n" +
                    std::string(strerror(errno)));
}

void Kqueue::enableEvent(uintptr_t ident, int16_t filter, void* udata) {
  struct kevent temp_event;

  EV_SET(&temp_event, ident, filter, EV_ENABLE, 0, 0, udata);
  int ret = kevent(Kqueue::_kq, &temp_event, 1, NULL, 0, NULL);
  if (ret == -1)
    throwWithPerror("kevent() error on enableEvent()\n" +
                    std::string(strerror(errno)));
}

void Kqueue::deleteEvent(uintptr_t ident, int16_t filter) {
  struct kevent temp_event;

  EV_SET(&temp_event, ident, filter, EV_DELETE, 0, 0, NULL);
  int ret = kevent(Kqueue::_kq, &temp_event, 1, NULL, 0, NULL);
  if (ret == -1)
    throwWithPerror("kevent() error on deleteEvent\n" +
                    std::string(strerror(errno)));
  deleteFdSet(ident, getFdType(ident));
}

int Kqueue::newEvents() {
  int new_events = kevent(_kq, &_eventsToAdd[0], _eventsToAdd.size(),
                          _eventList, CONCURRENT_EVENTS, NULL);
  if (new_events == -1)
    throwWithPerror("kevent() error\n" + std::string(strerror(errno)));
  _eventsToAdd.clear();
  return (new_events);
}

const struct kevent& Kqueue::getEvent(int index) const {
  return (this->_eventList[index]);
}

void Kqueue::init(void) {
  FD_ZERO(&_server_fds);
  FD_ZERO(&_client_fds);
  FD_ZERO(&_method_fds);
  FD_ZERO(&_cgi_fds);
}

e_fd_type Kqueue::getFdType(uintptr_t ident) {
  if (FD_ISSET(ident, &_server_fds))
    return (FD_SERVER);
  else if (FD_ISSET(ident, &_client_fds))
    return (FD_CLIENT);
  else if (FD_ISSET(ident, &_method_fds))
    return (FD_METHOD);
  else if (FD_ISSET(ident, &_cgi_fds))
    return (FD_CGI);
  return (FD_NONE);
}

void Kqueue::setFdSet(uintptr_t ident, e_fd_type type) {
  switch (type) {
    case FD_SERVER:
      FD_SET(ident, &_server_fds);
      break;
    case FD_CLIENT:
      FD_SET(ident, &_client_fds);
      break;
    case FD_METHOD:
      FD_SET(ident, &_method_fds);
      break;
    case FD_CGI:
      FD_SET(ident, &_cgi_fds);
      break;
    default:
      break;
  }
}

void Kqueue::deleteFdSet(uintptr_t ident, e_fd_type type) {
  switch (type) {
    case FD_SERVER:
      FD_CLR(ident, &_server_fds);
      break;
    case FD_CLIENT:
      FD_CLR(ident, &_client_fds);
      break;
    case FD_METHOD:
      FD_CLR(ident, &_method_fds);
      break;
    case FD_CGI:
      FD_CLR(ident, &_cgi_fds);
      break;
    default:
      break;
  }
}
