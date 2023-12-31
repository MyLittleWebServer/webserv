#ifndef POST_HPP
#define POST_HPP

#include <sys/stat.h>

#include "IMethod.hpp"

class POST : public IMethod {
 private:
  std::string _location;
  std::string _title;
  std::string _content;

  void generateResource(RequestDts& dts);

 public:
  POST();
  ~POST();

  void doRequest(RequestDts& dts, IResponse& response);
  void createSuccessResponse(IResponse& response);
  void generateUrlEncoded(RequestDts& dts);
  void generateMultipart(RequestDts& dts);
  void writeTextBody(RequestDts& dts, const std::string& mimeType);
  void writeBinaryBody(RequestDts& dts);

  std::string decodeURL(std::string encodedString);

  std::string makeRandomFileName(RequestDts& dts);

  bool getSpecificEndpoint(RequestDts& dts, IResponse& response);

 private:
  void handlePath(RequestDts& dts, IResponse& response);
  void login(RequestDts& dts, IResponse& response, Session& session);
  void enter(RequestDts& dts, IResponse& response, Session& session);
  void submit(RequestDts& dts, Session& session);
};
#endif
