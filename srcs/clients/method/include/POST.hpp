#ifndef POST_HPP
#define POST_HPP

#include "AMethod.hpp"

class POST : public AMethod {
 private:
  std::string _body;
  std::string _path;
  std::string _contentType;
  void generateResource();

 public:
  POST();
  POST(std::string& request);
  ~POST();

  void appendBody();
  void doRequest();
  void createSuccessResponse();
  std::string validateContentType();
};

#endif
