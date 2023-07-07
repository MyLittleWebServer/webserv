#include "FdInfo.hpp"

FdInfo::FdInfo() : _fdType(NONE), _readFd(-1), _writeFd(-1) {}
FdInfo::FdInfo(const FdInfo& src)
    : _fdType(src._fdType), _readFd(src._readFd), _writeFd(src._writeFd) {}

FdInfo& FdInfo::operator=(const FdInfo& src) {
  if (this != &src) {
    this->_fdType = src._fdType;
    this->_readFd = src._readFd;
    this->_writeFd = src._writeFd;
  }
  return *this;
}

FdInfo::~FdInfo() {}

FdType FdInfo::getFdType(void) const { return (this->_fdType); }
int FdInfo::getReadFd(void) const { return (this->_readFd); }
int FdInfo::getWriteFd(void) const { return (this->_writeFd); }

void FdInfo::setReadFd(int fd) {
  this->_fdType = FILE_READ;
  this->_readFd = fd;
}
void FdInfo::setWriteFd(int fd) {
  this->_fdType = FILE_WRITE;
  this->_writeFd = fd;
}
void FdInfo::setPipeFd(int readEnd, int writeEnd) {
  this->_fdType = PIPE;
  this->_readFd = readEnd;
  this->_writeFd = writeEnd;
}

bool FdInfo::isFileIO(void) const {
  return (this->_fdType == FILE_READ || this->_fdType == FILE_WRITE);
}
bool FdInfo::isPipeIO(void) const { return (this->_fdType == PIPE); }