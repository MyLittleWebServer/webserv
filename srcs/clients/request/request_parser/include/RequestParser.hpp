#pragma once

#include "IRequestParser.hpp"

class RequestParser : public IRequestParser {
 private:
  RequestParser();
  virtual ~RequestParser();
  RequestParser(const RequestParser& src);
  RequestParser& operator=(const RequestParser& src);

  void splitLinesByCRLF(RequestDts& dts);

  void parseRequestLine(RequestDts& dts);
  void parseHeaderFields(RequestDts& dts);

  std::string getFirstTokenOfPath(RequestDts& dts) const;
  bool checkBodyExistance(std::list<std::string>::const_iterator it,
                          RequestDts& dts) const;
  bool checkPathForm(RequestDts& dts);
  void setDefaultLocation(
      std::list<ILocationConfig*>::const_iterator defaultLocation,
      RequestDts& dts);

 private:
  const std::set<std::string>& _candidateFields;
  Status _statusCode;

 public:
  void parseRequest(RequestDts& dts);
  void matchServerConf(short port, RequestDts& dts);
  void validatePath(RequestDts& dts);

  static RequestParser& getInstance();
};
