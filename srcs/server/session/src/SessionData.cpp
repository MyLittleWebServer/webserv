#include "Session.hpp"

SessionData::SessionData() {}
SessionData::SessionData(const std::string& id, int time)
    : _id(id), _expirationTime(time) {
  _data["entername"] = "";
  _data["data"] = "";
  _data["fifteen"] = "off";
}
SessionData::SessionData(const SessionData& src)
    : _expirationTime(src._expirationTime), _data(src._data) {}
SessionData& SessionData::operator=(const SessionData& src) {
  if (this == &src) return *this;
  _expirationTime = src._expirationTime;
  _data = src._data;
  return *this;
}
SessionData::~SessionData() {}

const std::string& SessionData::getSessionId() const { return _id; }
int SessionData::getExpirationTime() const { return _expirationTime; }
const std::string& SessionData::getData(const std::string& key) const {
  return _data.at(key);
}

void SessionData::setData(const std::string& key, const std::string& value) {
  _data[key] = value;
}

bool SessionData::isKeyExist(const std::string& key) const {
  return (_data.find(key) != _data.end());
}