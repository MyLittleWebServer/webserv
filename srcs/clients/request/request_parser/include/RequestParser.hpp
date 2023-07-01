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
  void parseAnchor(RequestDts& dts, size_t pos);
  void parseQueryString(RequestDts& dts, size_t pos);
  void parseQueryKeyValue(RequestDts& dts, std::string str);
  void parseHeaderFields(RequestDts& dts);
  void checkContentLength(RequestDts& dts);

  std::string getFirstTokenOfPath(RequestDts& dts) const;
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
