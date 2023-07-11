#pragma once

#include "IRequestParser.hpp"
#include "Utils.hpp"

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
  void parseCgi(RequestDts& dts);
  void parseContent(RequestDts& dts);
  void parseContentLength(RequestDts& dts);
  void parseTransferEncoding(RequestDts& dts);
  void parseChunkedEncoding(RequestDts& dts);

  std::string getFirstTokenOfPath(RequestDts& dts) const;
  bool checkPathForm(RequestDts& dts);
  void setDefaultLocation(
      std::list<ILocationConfig*>::const_iterator defaultLocation,
      RequestDts& dts);
  void matchServerConf(short port, RequestDts& dts);
  void validatePath(RequestDts& dts);

 private:
  void requestChecker(RequestDts& dts);
  void checkProtocolVersion(RequestDts& dts);
  void checkContentLenghWithTransferEncoding(RequestDts& dts);
  void checkRequestUriLimitLength(RequestDts& dts);
  void checkHeaderLimitSize(RequestDts& dts);
  void checkBodyLimitLength(RequestDts& dts);
  void checkAllowedMethods(RequestDts& dts);
  void checkCgiMethod(RequestDts& dts);

 private:
  const std::set<std::string>& _candidateFields;

 public:
  void parseRequest(RequestDts& dts, short port);

  static RequestParser& getInstance();
};
