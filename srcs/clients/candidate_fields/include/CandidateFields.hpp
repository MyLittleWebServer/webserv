#pragma once

#include <set>
#include <string>

#include "ICandidateFields.hpp"

class CandidateFields : public ICandidateFields {
 private:
  CandidateFields();
  CandidateFields(const CandidateFields& src);
  CandidateFields& operator=(const CandidateFields& src);
  ~CandidateFields();

  void initCandidateFields();

 private:
  std::set<std::string> _candidateFields;

 public:
  static CandidateFields& getInstance();
  const std::set<std::string>& getCandidateFields();
};