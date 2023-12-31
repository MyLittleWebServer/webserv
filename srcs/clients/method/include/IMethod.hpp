#ifndef IMethod_HPP
#define IMethod_HPP

#include <sstream>
#include <string>

#include "Config.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "Session.hpp"
#include "Status.hpp"

class IMethod {
 public:
  virtual ~IMethod(){};
  virtual void doRequest(RequestDts& dts, IResponse& response) = 0;
  virtual void createSuccessResponse(IResponse& response) = 0;
};

#endif
