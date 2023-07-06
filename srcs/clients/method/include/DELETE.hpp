#ifndef DELETE_HPP
#define DELETE_HPP

#include <unistd.h>

#include <cstdio>
#include <iostream>

#include "IMethod.hpp"

class DELETE : public IMethod {
 public:
  DELETE();
  ~DELETE();

  void doRequest(RequestDts& dts, IResponse& response);
  void createSuccessResponse(IResponse& response);
};

#endif
