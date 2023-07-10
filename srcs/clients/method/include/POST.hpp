#ifndef POST_HPP
#define POST_HPP

#include "IMethod.hpp"

class POST : public IMethod {
 private:
  // void generateFile(RequestDts& dts);
  std::string _contentType;
  std::string _body;
  std::string _path;
  std::string _title;
  std::string _content;
  std::string _type;
  void generateResource(RequestDts& dts);

 public:
  POST();
  ~POST();

  void doRequest(RequestDts& dts, IResponse& response);
  void createSuccessResponse(IResponse& response);
  void generateUrlEncoded(RequestDts& dts);
  void generateMultipart(RequestDts& dts);
  void prepareTextBody(RequestDts& dts);
  void prepareBinaryBody(RequestDts& dts);

  std::string decodeURL(std::string encoded_string);

  // std::string validateContentType();
};

#endif
