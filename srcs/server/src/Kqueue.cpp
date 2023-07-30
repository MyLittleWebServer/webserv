/**
 * @file Kqueue.cpp
 * @brief FD, kevent를 관리하는 소스파일입니다.
 * @author TocaTocaToca
 * @date 2023-07-20
 * @copyright Copyright (c) 2023
 */

#include "Kqueue.hpp"

fd_set Kqueue::_server_fds;
fd_set Kqueue::_client_fds;
fd_set Kqueue::_method_fds;
fd_set Kqueue::_cgi_fds;

int Kqueue::__kq = 0;
std::vector<struct kevent> Kqueue::__eventsToAdd = std::vector<struct kevent>();
struct kevent Kqueue::__eventList[CONCURRENT_EVENTS] = {};

Kqueue::Kqueue(void) {
  if ((__kq = kqueue()) == -1) throwWithErrorMessage("kqueue error");
}

Kqueue::~Kqueue() {}

/**
 * @brief addEvent : kevent()를 호출하기 전에 이벤트를 _eventsToAdd에 등록
 *
 * @details
 * 해당 이벤트는 ident로 들어온 소켓 디스크립터를 event filter, flags, fflags,
 * data, udata로 설정합니다.
 *
 * @param ident : socket descriptor
 * @param filter : event filter(read/write)
 * @param flags : add, delete, enable, disable
 * @param fflags : field 유형의 flag
 * @param data : 이 필드는 필터 유형에 따라 특정한 데이터를 가집니다. 예를 들어,
 * EVFILT_READ인 경우 data 필드는 읽을 수 있는 데이터의 바이트 수를 나타냅니다.
 * @param udata : 사용자가 임의로 설정할 수 있는 포인터입니다. 이 필드는 kevent
 * 함수가 이벤트를 반환할 때 변경되지 않습니다.
 */
void Kqueue::addEvent(uintptr_t ident, int16_t filter, uint16_t flags,
                      uint32_t fflags, intptr_t data, void* udata) {
  struct kevent temp_event;

#ifdef DEBUG_MSG
  std::cout << "addEvent: ident: " << ident << " filter: " << filter
            << " flags: " << flags << " fflags: " << fflags << " data: " << data
            << " udata: " << udata << " " << std::endl;
#endif

  EV_SET(&temp_event, ident, filter, flags, fflags, data, udata);
  Kqueue::__eventsToAdd.push_back(temp_event);
}

/**
 * @brief addEvent kevent()를 호출하기 전에 서버 ident를 _eventsToAdd에 등록
 *
 * @details
 * 해당 이벤트는 서버를 추가할 때 사용합니다.
 *
 * @see EV_SET
 * @see kevent
 * @see EVFILT_READ
 * @see EV_ADD
 * @see EV_ENABLE
 *
 * @param ident : socket descriptor
 */
void Kqueue::addEvent(uintptr_t ident) {
  struct kevent temp_event;

  EV_SET(&temp_event, ident, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
  Kqueue::__eventsToAdd.push_back(temp_event);
}

void Kqueue::addSessionTimerEvent() {
  struct kevent temp_event;

  EV_SET(&temp_event, SESSION_TIMER, EVFILT_TIMER, EV_ADD | EV_ENABLE,
         NOTE_SECONDS, 600, (void*)NULL);
  Kqueue::__eventsToAdd.push_back(temp_event);
}

void Kqueue::registEvent(uintptr_t ident, int16_t filter, uint16_t flags,
                         uint32_t fflags, intptr_t data, void* udata) {
  struct kevent temp_event;

  EV_SET(&temp_event, ident, filter, flags, fflags, data, udata);
  int ret = kevent(Kqueue::__kq, &temp_event, 1, NULL, 0, NULL);
  if (ret == -1)
    throwWithErrorMessage("kevent() error on registEvent()\n" +
                          std::string(strerror(errno)));
}

/**
 * @brief disableEvent function
 *
 * @details
 * disableEvent 함수는 ident의 filter 이벤트를 비활성화합니다.
 *
 * @see EV_SET
 * @see kevent
 *
 * @param ident
 * @param filter
 * @param udata
 */
void Kqueue::disableEvent(uintptr_t ident, int16_t filter, void* udata) {
  struct kevent temp_event;

  EV_SET(&temp_event, ident, filter, EV_DISABLE, 0, 0, udata);
  int ret = kevent(Kqueue::__kq, &temp_event, 1, NULL, 0, NULL);
  if (ret == -1) throwWithErrorMessage("kevent error on disableEvent");
}

/**
 * @brief enableEvent function
 * enableEvent 함수는 ident의 filter 이벤트를 활성화합니다.
 * @param ident socket descriptor
 * @param filter event filter(read/write or etc)
 * @param udata 사용자가 임의로 설정할 수 있는 포인터입니다. 이 필드는 kevent
 * 함수가 이벤트를 반환할 때 변경되지 않습니다.
 */
void Kqueue::enableEvent(uintptr_t ident, int16_t filter, void* udata) {
  struct kevent temp_event;

  EV_SET(&temp_event, ident, filter, EV_ENABLE, 0, 0, udata);
  int ret = kevent(Kqueue::__kq, &temp_event, 1, NULL, 0, NULL);
  if (ret == -1) throwWithErrorMessage("kevent error on enableEvent");
}

/**
 * @brief deleteEvent function
 * @param ident socket descriptor
 * @param filter event filter(read/write or etc)
 * @param udata 사용자가 임의로 설정할 수 있는 포인터입니다. 이 필드는 kevent
 * 함수가 이벤트를 반환할 때 변경되지 않습니다.
 */
void Kqueue::deleteEvent(uintptr_t ident, int16_t filter, void* udata) {
  struct kevent temp_event;
  EV_SET(&temp_event, ident, filter, EV_DELETE, 0, 0, udata);
  int ret = kevent(Kqueue::__kq, &temp_event, 1, NULL, 0, NULL);
  if (ret == -1) std::cout << "kevent error on deleteEvent" << std::endl;
}

/**
 * @brief newEvents 함수는 새로운 이벤트를 반환합니다.
 *
 * @details
 * 1. kevent() 함수를 호출하여 _kq에 등록된 이벤트가 있는지 확인합니다. 이벤트는
 * _eventsToAdd에 등록되며, 새로 발생한 이벤트는 _eventList에 저장됩니다.
 * _eventToAdd.size()는 등록된 이벤트의 수를 나타내고 CONCURRENT_EVENTS는 한번에
 * 처리할 수 있는 이벤트의 최대 수를 나타냅니다. 마지막 매개변수 NULL은
 * timeout에 대한 설정을 나타냅니다.
 *
 * 2. kevent() 함수 호출이 성공하면, 새로운 이벤트의 수를 반환합니다. 그리고
 * _eventToAdd를 비웁니다.
 *
 * @exception kevent() error : kevent() 함수가 -1을 반환하면 에러입니다.
 *
 * @return int : 새로운 이벤트의 수
 */
int Kqueue::newEvents() {
  int new_events = kevent(__kq, &__eventsToAdd[0], __eventsToAdd.size(),
                          __eventList, CONCURRENT_EVENTS, NULL);
  if (new_events == -1) throwWithErrorMessage("kevent error on newEvents");
  __eventsToAdd.clear();
  return (new_events);
}

/**
 * @brief getEvent 이벤트 리스트의 index번째 이벤트를 반환합니다.
 *
 * @param index
 * @return const struct kevent&
 */
const struct kevent& Kqueue::getEvent(int index) const {
  return (__eventList[index]);
}

/**
 * @brief init function
 * fd_set 집합들을 초기화합니다.
 */
void Kqueue::init(void) {
  FD_ZERO(&Kqueue::_server_fds);
  FD_ZERO(&Kqueue::_client_fds);
  FD_ZERO(&Kqueue::_method_fds);
  FD_ZERO(&Kqueue::_cgi_fds);
}

/**
 * @brief getFdType function
 *
 * @details
 * fd_set 집합들 중 어느 집합에 속하는지 확인합니다.
 *
 * @param ident socket descriptor
 * @return e_fd_type : fd_set type(FD_SERVER, FD_CLIENT, FD_METHOD, FD_CGI)
 */
e_fd_type Kqueue::getFdType(uintptr_t ident) {
  if (FD_ISSET(ident, &Kqueue::_server_fds)) {
#ifdef DEBUG_MSG
    std::cout << "getFdType: " << ident << " in server" << std::endl;
#endif
    return (FD_SERVER);
  } else if (FD_ISSET(ident, &Kqueue::_client_fds)) {
#ifdef DEBUG_MSG
    std::cout << "getFdType: " << ident << " in client" << std::endl;
#endif
    return (FD_CLIENT);
  } else if (FD_ISSET(ident, &Kqueue::_method_fds)) {
#ifdef DEBUG_MSG
    std::cout << "getFdType: " << ident << " in method" << std::endl;
#endif
    return (FD_METHOD);
  } else if (FD_ISSET(ident, &Kqueue::_cgi_fds)) {
#ifdef DEBUG_MSG
    std::cout << "getFdType: " << ident << " in cgi" << std::endl;
#endif
    return (FD_CGI);
  }
#ifdef DEBUG_MSG
  std::cout << "getFdType: " << ident << " is none" << std::endl;
#endif
  return (FD_NONE);
}

/**
 * @brief setFdSet function
 *
 * @details
 * 집합에서 특정 파일 디스크립터 FD를 추가하는 데 사용됩니다.
 *
 * @param ident socket descriptor
 * @param type fd_set type
 */
void Kqueue::setFdSet(uintptr_t ident, e_fd_type type) {
  switch (type) {
    case FD_SERVER:
      FD_SET(ident, &Kqueue::_server_fds);
      break;
    case FD_CLIENT:
      FD_SET(ident, &Kqueue::_client_fds);
      break;
    case FD_METHOD:
      FD_SET(ident, &Kqueue::_method_fds);
      break;
    case FD_CGI:
      FD_SET(ident, &Kqueue::_cgi_fds);
      break;
    default:
      break;
  }
}

/**
 * @brief deleteFdSet function
 *
 * @details
 * 집합에서 특정 파일 디스크립터 FD를 제거하는 데 사용됩니다.
 *
 * @param ident socket descriptor
 * @param type fd_set type
 */
void Kqueue::deleteFdSet(uintptr_t ident, e_fd_type type) {
  switch (type) {
    case FD_SERVER: {
      // std::cout << "deleteFdSet: " << ident << "FD_SERVER" << std::endl;
      FD_CLR(ident, &Kqueue::_server_fds);
      break;
    }
    case FD_CLIENT: {
      // std::cout << "deleteFdSet: " << ident << "FD_CLIENT" << std::endl;
      FD_CLR(ident, &Kqueue::_client_fds);
      break;
    }
    case FD_METHOD: {
      // std::cout << "deleteFdSet: " << ident << "FD_METHOD" << std::endl;
      FD_CLR(ident, &Kqueue::_method_fds);
      break;
    }
    case FD_CGI: {
      // std::cout << "deleteFdSet: " << ident << "FD_CGI" << std::endl;
      FD_CLR(ident, &Kqueue::_cgi_fds);
      break;
    }
    default:
      break;
  }
#ifdef DEBUG_MSG
  getFdType(ident);
#endif
}
