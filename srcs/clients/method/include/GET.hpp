#ifndef GET_HPP
#define GET_HPP

#include "IMethod.hpp"

class GET : public IMethod {
 private:
  std::string _body;
  std::string _contentType;

  std::vector<std::string> getFileList(const std::string& path,
                                       RequestDts& dts);
  std::string generateHTML(const std::vector<std::string>& files);
  std::string validateContentType();
  void prepareFileList(const std::string& path, RequestDts& dts);
  void prepareBody(const std::string& pathIndex);
  void getContentType(const std::string& path);

  void prepareTextBody(const std::string& path);
  void prepareBinaryBody(const std::string& path);

 public:
  GET();
  ~GET();

  void doRequest(RequestDts& dts);
  void appendBody(IResponse& response);
  void fileHandler(const std::string& path);
  void createSuccessResponse(IResponse& response);
};

#endif
