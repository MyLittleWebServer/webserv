#ifndef GET_HPP
#define GET_HPP

#include "AMethod.hpp"

class GET : public AMethod {
 private:
  std::string _fileContent;
  void prepareFileList(const std::string& path);
  void prepareFileContent(const std::string& pathIndex);

 public:
  GET();
  GET(std::string& request);
  ~GET();

  void doRequest();
  void appendFileContent();
};

#endif
