#ifndef POST_HPP
#define POST_HPP

#include "IMethod.hpp"

class POST : public IMethod {
 private:
  void generateFile(RequestDts& dts);
  std::string _body;
  std::string _contentType;
  std::string _path;
  std::string _title;
  std::string _content;
  std::string _disposName;
  std::string _disposFilename;
  std::string _type;
  void generateResource(RequestDts& dts, IResponse& response);

 public:
  POST();
  ~POST();

  void doRequest(RequestDts& dts);
  void createSuccessResponse(IResponse& response);
  void appendBody();
  void generateUrlEncoded(RequestDts& dts, IResponse& response);
  void generateMultipart(RequestDts& dts, IResponse& response);
  void prepareTextBody(const std::string& body);
  void prepareBinaryBody(const std::string& filename);
  void createDisposSuccessResponse(IResponse& response);
  std::string createHTML(std::string const& head);

  std::string decodeURL(std::string const& encoded_string);

  // std::string validateContentType();
};

#endif
