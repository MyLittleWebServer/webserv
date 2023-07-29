#ifndef GET_HPP
#define GET_HPP

#include "IMethod.hpp"

class GET : public IMethod {
 private:
  std::string _body;
  std::string _contentType;

 private:
  void handlePath(RequestDts& dts, IResponse& response);
  void getPublicEndpoint(RequestDts& dts, IResponse& response);
  bool getSpecificEndpoint(RequestDts& dts, IResponse& response);
  bool checkFile(std::string& path);
  bool checkDirectory(std::string& path);
  bool checkIndexFile(std::string& pathIndex);
  bool checkAutoIndex(std::string& pathIndex, const std::string& autoindex);
  void prepareBody(const std::string& pathIndex, IResponse& response);
  void prepareFileList(const std::string& path, RequestDts& dts,
                       IResponse& response);
  std::vector<std::string> getFileList(const std::string& path,
                                       RequestDts& dts);
  std::string generateHTML(const std::vector<std::string>& files);
  void getContentType(const std::string& path, IResponse& response);

  void prepareTextBody(const std::string& path, IResponse& response);
  void prepareBinaryBody(const std::string& path, IResponse& response);

 private:
  void getSessionData(IResponse& response, SessionData& sessionData);
  void getEnterPage(RequestDts& dts);
  void getJangCho(RequestDts& dts);
  void getJangChoOrMarin(RequestDts& dts, SessionData& sessionData);
  void getHome(RequestDts& dts);
  void getGaepo(RequestDts& dts, SessionData& sessionData);

 protected:
  void validateContentType(IResponse& response);

 public:
  GET();
  virtual ~GET();

  void doRequest(RequestDts& dts, IResponse& response);
  void fileHandler(const std::string& path);
  virtual void createSuccessResponse(IResponse& response);
};

#endif
