#include "CandidateFields.hpp"

CandidateFields::CandidateFields() { initCandidateFields(); }

CandidateFields::CandidateFields(const CandidateFields& src) { *this = src; }

CandidateFields& CandidateFields::operator=(const CandidateFields& src) {
  if (this != &src) {
    _candidateFields = src._candidateFields;
  }
  return *this;
}

CandidateFields::~CandidateFields() {}

void CandidateFields::initCandidateFields() {
  _candidateFields.insert("host");
  _candidateFields.insert("accept");
  _candidateFields.insert("transfer-encoding");
  _candidateFields.insert("accept-language");
  _candidateFields.insert("accept-encoding");
  _candidateFields.insert("accept-charset");
  _candidateFields.insert("authorization");
  _candidateFields.insert("content-type");
  _candidateFields.insert("connection");
  _candidateFields.insert("user-agent");
  _candidateFields.insert("content-length");
  _candidateFields.insert("content-language");
  _candidateFields.insert("content-encoding");
  _candidateFields.insert("content-range");
  _candidateFields.insert("content-length");
  _candidateFields.insert("content-base");
  _candidateFields.insert("content-location");
  _candidateFields.insert("content-range");
  _candidateFields.insert("keep-alive");
  _candidateFields.insert("referer");
  _candidateFields.insert("cookie");
  _candidateFields.insert("last-modified");
  _candidateFields.insert("if-modified-since");
  _candidateFields.insert("date");
  _candidateFields.insert("server");
  _candidateFields.insert("www-authenticate");
  _candidateFields.insert("retry-after");
  _candidateFields.insert("location");
  _candidateFields.insert("content-md5");
  _candidateFields.insert("cache-control");
  _candidateFields.insert("pragma");
  _candidateFields.insert("expires");
  _candidateFields.insert("age");
  _candidateFields.insert("allow");
  _candidateFields.insert("etag");
  _candidateFields.insert("accept-ranges");
  _candidateFields.insert("set-cookie");
  _candidateFields.insert("vary");
  _candidateFields.insert("x-frame-options");
  _candidateFields.insert("x-xss-protection");
  _candidateFields.insert("x-content-type-options");
  _candidateFields.insert("x-forwarded-server");
  _candidateFields.insert("x-forwarded-proto");
  _candidateFields.insert("x-real-ip");
  _candidateFields.insert("x-request-id");
  _candidateFields.insert("x-correlation-id");
  _candidateFields.insert("x-csrf-token");
  _candidateFields.insert("x-device-user-agent");
  _candidateFields.insert("te");
}

CandidateFields& CandidateFields::getInstance() {
  static CandidateFields instance;
  return instance;
}

const std::set<std::string>& CandidateFields::getCandidateFields() {
  return _candidateFields;
}