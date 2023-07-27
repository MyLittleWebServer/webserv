#pragma once

#include <map>
#include <string>

#include "SessionData.hpp"
#include "Utils.hpp"

class Session {
 private:
  std::map<std::string, SessionData> _sessionData;

  Session();
  Session(const Session& src);
  Session& operator=(const Session& src);
  ~Session();

  bool isExpired(const std::string& sessionId);
  void eraseSessionData(const std::string& sessionId);

 public:
  static Session& getInstance();
  std::string createSession(int expirationTime);
  SessionData& getSessionData(const std::string& sessionId);
  bool isSessionExist(const std::string& sessionId);
  void deleteExpiredSessions();
};