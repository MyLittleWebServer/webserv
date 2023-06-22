#ifndef POST_HPP
#define POST_HPP

#include "AMethod.hpp"

class POST : public AMethod {
 private:
  void generateFile();

 public:
  POST();
  POST(std::string& request);
  ~POST();

  void doMethod();
  void createResponse();
};

#endif
