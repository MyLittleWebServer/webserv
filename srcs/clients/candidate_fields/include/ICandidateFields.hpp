#pragma once

#include <set>
#include <string>

class ICandidateFields {
 public:
  virtual const std::set<std::string>& getCandidateFields() = 0;

  virtual ~ICandidateFields() {}
};