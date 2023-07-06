#ifndef DUMMYMETHOD_HPP
#define DUMMYMETHOD_HPP

#include "IMethod.hpp"

class DummyMethod : public IMethod {
 public:
  DummyMethod(Status statusCode);
  ~DummyMethod();

  void doRequest(RequestDts& dts, IResponse& response);
  void createSuccessResponse(IResponse& response);
};

#endif