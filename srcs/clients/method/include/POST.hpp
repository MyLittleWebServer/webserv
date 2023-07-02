#ifndef POST_HPP
#define POST_HPP

#include "AMethod.hpp"

class POST : public AMethod {
 private:
  std::string _title;
  std::string _content;

  std::string _body;
  std::string _path;
  std::string _contentType;
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
