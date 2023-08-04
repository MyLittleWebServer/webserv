#ifndef GET_HPP
#define GET_HPP

#include "IMethod.hpp"

#define HTML_HEAD_TITLE(x) \
  "<html>\r\n<head><title>Index of " + x + "</title><meta charset=\"utf-8\">"
#define HTML_BODY(x) "<body>\r\n<h1>Index of " + x + "</h1>\r\n"
#define STYLE                                               \
  "<style> body { padding: 2rem; line-height : "            \
  "2.5rem; } h1 { margin-bottom : 2rem; } a { margin-top: " \
  "0.3rem; "                                                \
  "margin-bottom: 0.5rem; display: block;}</style></head>\r\n "
#define HTML_BODY_END "\r\n</body>\r\n</html>"

class GET : public IMethod {
 private:
  std::string _body;
  std::string _contentType;

 private:
  void handlePath(RequestDts& dts, IResponse& response);
  void getPublicEndpoint(RequestDts& dts, IResponse& response);
  bool getSpecificEndpoint(RequestDts& dts, IResponse& response);
  bool checkFile(const std::string& path);
  bool checkDirectory(const std::string& path);
  bool checkIndexFile(const std::string& pathIndex);
  bool checkAutoIndex(std::string& pathIndex, const std::string& autoindex);
  void prepareBody(RequestDts& dts, const std::string& pathIndex,
                   IResponse& response);
  void prepareFileList(const std::string& path, RequestDts& dts,
                       IResponse& response);
  std::vector<std::string> getFileList(const std::string& path,
                                       RequestDts& dts);
  std::string generateHTML(const std::string& path,
                           const std::vector<std::string>& files);
  void getContentType(const std::string& path, IResponse& response);
  void prepareTextBody(RequestDts& dts, const std::string& path,
                       IResponse& response);
  void prepareBinaryBody(RequestDts& dts, const std::string& path,
                         IResponse& response);
  void bodyCheck(RequestDts& dts, IResponse& response);

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
  virtual void createSuccessResponse(IResponse& response);
};

#endif
