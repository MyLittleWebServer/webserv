#ifndef POST_HPP
#define POST_HPP

#include "AMethod.hpp"

class POST : public AMethod {
 public:
  POST() {}
  POST(std::string &request) : AMethod(request) {}
  ~POST() {}

  void parseRequestMessage() {}
  void createResponseMessage() {}
};

#endif
