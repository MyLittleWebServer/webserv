#ifndef GET_HPP
#define GET_HPP

#include "AMethod.hpp"

class GET : public AMethod {
 private:
  std::string _body;
  bool _autoIndex;

  void prepareFileList(const std::string& path);
  void prepareBody(const std::string& pathIndex);

 public:
  GET();
  GET(std::string& request);
  ~GET();

  void doRequest();
  void appendBody();
  void createSuccessResponse();
};

#endif
