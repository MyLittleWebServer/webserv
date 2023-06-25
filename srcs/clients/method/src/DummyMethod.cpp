#include "DummyMethod.hpp"

DummyMethod::DummyMethod(Status statusCode) : AMethod(statusCode) {}
DummyMethod::~DummyMethod() {}

void DummyMethod::doRequest(void) {}
void DummyMethod::createSuccessResponse(void) {}