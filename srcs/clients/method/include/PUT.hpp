#ifndef PUT_HPP
#define PUT_HPP

// #include <vector>
#include <sys/stat.h>

#include "IMethod.hpp"

class PUT : public IMethod {
 private:
  std::string _path;
  std::string _uniqueID;
  std::string _uniqueContent;

  std::string _contentType;
  std::string _boundary;
  std::string _title;
  std::string _content;

  std::string _alterContent;
  std::string _pathFinder;

  struct stat fileinfo;

  void generateResource(RequestDts& dts);

 public:
  PUT();
  ~PUT();

  void doRequest(RequestDts& dts, IResponse& response);
  void createSuccessResponse(IResponse& response);

  void generateUrlEncoded(RequestDts& dts);
  void generateMultipart(RequestDts& dts);
  void writeTextBody(RequestDts& dts);
  void writeBinaryBody(RequestDts& dts);

  std::string decodeURL(std::string encoded_string);
  std::string makeRandomFileName(RequestDts& dts);

  /* PUT only */
  void initUniqueIdentifier(RequestDts& dts);

  /* check for Content */
  bool checkBodyContent(RequestDts& dts);
  bool checkForUrlEncoded(RequestDts& dts);
  bool checkForPlainText(RequestDts& dts);
  bool checkForMultipart(RequestDts& dts);

  /* replace for Content */
  void replaceContent(RequestDts& dts);
};

#endif