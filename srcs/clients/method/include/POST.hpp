#ifndef POST_HPP
#define POST_HPP

#include <sys/stat.h>

#include "IMethod.hpp"

class POST : public IMethod {
 private:
  std::string _contentType;
  std::string _boundary;
  std::string _title;
  std::string _content;
  std::string _randName;

  struct stat fileinfo;

  void generateResource(RequestDts& dts);

 public:
  POST();
  ~POST();

  void doRequest(RequestDts& dts, IResponse& response);
  void createSuccessResponse(IResponse& response);
  void generateUrlEncoded(RequestDts& dts);
  void generateMultipart(RequestDts& dts);
  void writeTextBody(RequestDts& dts, std::string mimeType);
  void writeBinaryBody(RequestDts& dts);

  std::string decodeURL(std::string encoded_string);

  std::string makeRandomFileName(RequestDts& dts);

  // std::string validateContentType();
};

#endif
