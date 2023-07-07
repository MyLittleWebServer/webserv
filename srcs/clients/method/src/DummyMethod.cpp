#include "DummyMethod.hpp"

DummyMethod::DummyMethod(Status statusCode) { (void)statusCode; }
DummyMethod::~DummyMethod() {}

void DummyMethod::doRequest(RequestDts& dts,IResponse &response, FdInfo &fdInfo) { (void)dts;
  (void)response; (void)fdInfo;
}
void DummyMethod::createSuccessResponse(IResponse& response) { (void)response; }