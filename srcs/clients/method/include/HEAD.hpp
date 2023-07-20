#pragma once

#include "GET.hpp"

class HEAD : public GET {
 private:
  void generateResource(RequestDts& dts);

 public:
  HEAD();
  ~HEAD();

  void createSuccessResponse(IResponse& response);
};
