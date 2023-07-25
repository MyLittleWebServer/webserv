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
  std::string newSessionId = generateRandomString();
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