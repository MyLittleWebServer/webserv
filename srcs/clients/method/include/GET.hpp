#ifndef GET_HPP
#define GET_HPP

#include "AMethod.hpp"

class GET : public AMethod {
 private:
  std::string _body;
  std::string _contentType;

  std::vector<std::string> getFileList(const std::string& path);
  std::string generateHTML(const std::vector<std::string>& files);
  std::string validateContentType();
  void prepareFileList(const std::string& path);
  void prepareBody(const std::string& pathIndex);
  void getContentType(const std::string& path);

  void prepareTextBody(const std::string& path);
  void prepareBinaryBody(const std::string& path);
  
 public:
  GET();
  GET(std::string& request);
  ~GET();

  void doRequest();
  void appendBody();
  void createSuccessResponse();
};

#endif
