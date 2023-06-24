#ifndef DUMMYMETHOD_HPP
#define DUMMYMETHOD_HPP

#include "AMethod.hpp"
#include "Status.hpp"

class DummyMethod : public AMethod {
 public:
  DummyMethod(Status statusCode);
  ~DummyMethod();

  void doRequest(void);
  void createSuccessResponse(void);
};

#endif