#ifndef AMethod_HPP
#define AMethod_HPP

#include <list>
#include <map>
#include <sstream>
#include <string>

class AMethod {
 protected:
  std::string _request;
  std::string _response;
  bool _responseFlag;

  std::string _method;
  std::string _path;
  std::string _protocol;

  std::list<std::string> _linesBuffer;
  std::map<std::string, std::string> _headerFields;

  void splitLinesByCRLF(void);
  void parseRequestLine(void);
  void parseHeaderFields(void);
  bool checkBodyExistance(const std::string &str) const;

 public:
  AMethod();
  AMethod(std::string &request);
  virtual ~AMethod();
  virtual void parseRequestMessage(void) = 0;
  virtual void createResponseMessage(void) = 0;

  const std::string &getResponse(void) const;
  bool getResponseFlag(void) const;
};

#endif
