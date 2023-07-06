#include "DummyMethod.hpp"

DummyMethod::DummyMethod(Status statusCode) { (void)statusCode; }
DummyMethod::~DummyMethod() {}

void DummyMethod::doRequest(RequestDts& dts,IResponse &response ) { (void)dts;
  (void)response;
}
void DummyMethod::createSuccessResponse(IResponse& response) { (void)response; }