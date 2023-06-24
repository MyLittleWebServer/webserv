#include "DummyMethod.hpp"

DummyMethod::DummyMethod(Status statusCode) {
  this->_statusCode = NOT_IMPLEMENTED;
}
DummyMethod::~DummyMethod() {}

void DummyMethod::doRequest(void) {}
void DummyMethod::createSuccessResponse(void) {}