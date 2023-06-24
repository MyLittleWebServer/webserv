#ifndef DELETE_HPP
#define DELETE_HPP

#include <unistd.h>

#include <cstdio>
#include <iostream>

#include "AMethod.hpp"
#include "Status.hpp"

class DELETE : public AMethod {
 public:
  DELETE();
  DELETE(std::string &request);
  ~DELETE();

  void doRequest(void);
  void createSuccessResponse(void);
};

#endif
