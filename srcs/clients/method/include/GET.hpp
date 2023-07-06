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
  void validateContentType(IResponse &response);
  void prepareFileList(const std::string& path, RequestDts& dts,
                       IResponse& response);
  void prepareBody(const std::string& pathIndex, IResponse& response);
  void getContentType(const std::string& path, IResponse &response);

  void prepareTextBody(const std::string& path, IResponse& response);
  void prepareBinaryBody(const std::string& path, IResponse& response);

 public:
  GET();
  ~GET();

  void doRequest(RequestDts& dts, IResponse& response);
  void fileHandler(const std::string& path);
  void createSuccessResponse(IResponse& response);
};

#endif
