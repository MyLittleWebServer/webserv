#pragma once

enum FdType { NONE, FILE_READ, FILE_WRITE, PIPE };

class FdInfo {
 private:
  FdType _fdType;
  int _readFd;
  int _writeFd;

 public:
  FdInfo();
  FdInfo(const FdInfo& src);
  FdInfo& operator=(const FdInfo& src);
  ~FdInfo();

  FdType getFdType(void) const;
  int getReadFd(void) const;
  int getWriteFd(void) const;

  void setReadFd(int fd);
  void setWriteFd(int fd);
  void setPipeFd(int readEnd, int writeEnd);

  bool isFileIO(void) const;
  bool isPipeIO(void) const;
};