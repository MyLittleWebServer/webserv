#pragma once

#include "IMethod.hpp"

class OPTIONS : public IMethod {
 private:
  void makeServerOptions(IResponse& response);
  void makeLocationOption(RequestDts& dts, IResponse& response);

 public:
  OPTIONS();
  virtual ~OPTIONS();

  void doRequest(RequestDts& dts, IResponse& response);
  void createSuccessResponse(IResponse& response);
};
