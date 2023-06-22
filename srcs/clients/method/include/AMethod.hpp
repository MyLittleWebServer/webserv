#ifndef AMethod_HPP
#define AMethod_HPP

#include <list>
#include <map>
#include <sstream>
#include <string>

#include "Config.hpp"
#include "GET.hpp"

class AMethod {
 protected:
  std::string _request;
  std::string _response;
  bool _responseFlag;

  std::string _method;
  std::string _path;
  std::string _protocol;
  std::string _CGI;
  int _statusCode;

  std::list<std::string> _linesBuffer;
  std::map<std::string, std::string> _headerFields;
  std::map<std::string, std::string> _serverConf;
  IServerConfig *_matchedServer;
  ILocationConfig *_matchedLocation;

  void splitLinesByCRLF(void);
  void parseRequestLine(void);
  void parseHeaderFields(void);

  bool checkBodyExistance(std::list<std::string>::const_iterator it) const;
  bool checkPathForm();
  void setDefaultLocation(
      std::list<ILocationConfig *>::const_iterator defaultLocation);

 public:
  AMethod();
  AMethod(std::string &request);
  virtual ~AMethod();

  void parseRequest(void);
  void matchServerConf(short port);
  void validatePath(void);
  virtual void doRequest(void) = 0;
  void createResponse(void);

  const std::string &getResponse(void) const;
  bool getResponseFlag(void) const;
};

#endif
