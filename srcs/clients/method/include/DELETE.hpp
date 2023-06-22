#ifndef DELETE_HPP
#define DELETE_HPP

#include <unistd.h>

#include <cstdio>
#include <iostream>

#include "AMethod.hpp"

class DELETE : public AMethod {
 public:
  DELETE(){};
  DELETE(std::string &request) {}
  ~DELETE(){};

  void doRequest() {}
  void createResponse() {}
};

#endif
