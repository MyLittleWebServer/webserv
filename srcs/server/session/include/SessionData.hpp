#pragma once

#include <map>
#include <string>

class SessionData {
 private:
  const std::string _id;
  int _expirationTime;
  std::map<std::string, std::string> _data;

 public:
  SessionData();
  SessionData(const std::string& id, int time);
  SessionData(const SessionData& src);
  SessionData& operator=(const SessionData& src);
  ~SessionData();

  const std::string& getSessionId() const;
  int getExpirationTime() const;
  const std::string& getData(const std::string& key) const;

  void setData(const std::string& key, const std::string& value);
};