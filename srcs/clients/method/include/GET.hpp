#ifndef GET_HPP
#define GET_HPP

#include "AMethod.hpp"

class GET : public AMethod {
 private:
  void getFilePath(void);

 public:
  GET();
  GET(std::string &request);
  ~GET();

  void parseRequestMessage();
  void createResponseMessage();
};

#endif
