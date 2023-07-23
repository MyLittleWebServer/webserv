#ifndef PUT_HPP
#define PUT_HPP

#include <vector>

#include "IMethod.hpp"

class PUT : public IMethod {
 private:
  std::string _path;
  std::string _title;
  std::string _content;
  std::vector<std::string> _uniqueIDs;

  std::string _boundary;
  std::string _contentType;

 public:
  PUT();
  ~PUT();

  void doRequest(RequestDts& dts, IResponse& response);
  void createSuccessResponse(IResponse& response);
  void checkUniqueIdentifier(RequestDts& dts);
};

#endif