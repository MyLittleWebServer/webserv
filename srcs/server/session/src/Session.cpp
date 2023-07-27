#include "Session.hpp"

#include "ExceptionThrower.hpp"

Session::Session() {}
Session::Session(const Session& src) { (void)src; }
Session& Session::operator=(const Session& src) {
  (void)src;
  return *this;
}
Session::~Session() {}

Session& Session::getInstance() {
  static Session instance;
  return instance;
}

std::string Session::createSession(int expirationTime) {
  std::string newSessionId = toLowerString(generateRandomString());
  SessionData temp(newSessionId, expirationTime);
  _sessionData[newSessionId] = temp;
  return newSessionId;
}

SessionData& Session::getSessionData(const std::string& sessionId) {
  if (isSessionExist(sessionId) == false) {
    throw ExceptionThrower::SessionDataNotFound();
  }
  if (isExpired(sessionId)) {
    eraseSessionData(sessionId);
    throw ExceptionThrower::SessionDataExpired();
  }
  return _sessionData[sessionId];
}

void Session::eraseSessionData(const std::string& sessionId) {
  _sessionData.erase(sessionId);
}

bool Session::isSessionExist(const std::string& sessionId) {
  if (_sessionData.find(sessionId) != _sessionData.end()) {
    return true;
  }
  return false;
}

bool Session::isExpired(const std::string& sessionId) {
  int currentTime = getTimeOfDay();

  if (currentTime > _sessionData[sessionId].getExpirationTime()) {
    return true;
  }
  return false;
}

void Session::deleteExpiredSessions() {
  if (_sessionData.empty()) {
    return;
  }
  std::map<std::string, SessionData>::iterator it = _sessionData.begin();
  std::map<std::string, SessionData>::iterator end = _sessionData.end();
  std::map<std::string, SessionData>::iterator next;
  for (; it != end; ++it) {
    next = it;
    ++next;
    if (isExpired(it->first)) {
      _sessionData.erase(it);
    }
    it = next;
  }
}