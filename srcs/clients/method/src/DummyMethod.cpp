#include "DummyMethod.hpp"

DummyMethod::DummyMethod(Status statusCode) { (void)statusCode; }
DummyMethod::~DummyMethod() {}

void DummyMethod::doRequest(RequestDts& dts) { (void)dts; }
void DummyMethod::createSuccessResponse(IResponse& response) { (void)response; }