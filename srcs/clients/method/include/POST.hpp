#ifndef POST_HPP
#define POST_HPP

#include "AMethod.hpp"

class POST : public AMethod {
 private:
  std::string _body;
  std::string _contentType;
  std::string _path;
  /* urlEncoded */
  std::string _title;
  std::string _content;
  /* multipart/form-data */
  std::string _disposName;
  std::string _disposFilename;
  std::string _type;
  void generateResource();

 public:
  POST();
  POST(std::string& request);
  ~POST();

  void appendBody();
  void doRequest();
  void createSuccessResponse();
  void generateUrlEncoded();
  void generateMultipart();
  std::string createHTML(std::string const& title);

  std::string decodeURL(std::string const& encoded_string);
  // void generateTextPlain();

  std::string validateContentType();
};

#endif
