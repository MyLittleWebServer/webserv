#pragma once

#include "IRequestParser.hpp"
#include "Utils.hpp"

class RequestParser : public IRequestParser {
 private:
  bool _valid_flag;
  RequestParser();
  virtual ~RequestParser();
  RequestParser(const RequestParser& src);
  RequestParser& operator=(const RequestParser& src);

 private:
  void splitLinesByCRLF(RequestDts& dts);
  void parseRequestLine(RequestDts& dts);
  void parseAnchor(RequestDts& dts);
  void parseQueryString(RequestDts& dts);
  void parseQueryKeyValue(RequestDts& dts, std::string str);
  void parseHeaderFields(RequestDts& dts);
  void parseCookie(RequestDts& dts);
  void parseCgi(RequestDts& dts);
  void parseSessionConfig(RequestDts& dts);
  void parseContent(RequestDts& dts);
  void parseContentLength(RequestDts& dts);
  void parseTransferEncoding(RequestDts& dts);
  void parseChunkedEncoding(RequestDts& dts);
  std::string getFirstTokenOfPath(RequestDts& dts) const;
  bool allHeaderRecieved(RequestDts& dts);
  void setDefaultLocation(
      std::list<ILocationConfig*>::const_iterator defaultLocation,
      RequestDts& dts);
  void matchServerConf(short port, RequestDts& dts);
  void validatePath(RequestDts& dts);
  void checkAndParseRedirection(RequestDts& dts);

 private:
  void attackGuard(RequestDts& dts);
  void setConnectionClose(RequestDts& dts);
  void validateDuplicateInvalidHeaders(std::string key, RequestDts& dts);
  void validateContentLengthHeader(RequestDts& dts);
  void validateHeaderKey(std::string& key, RequestDts& dts);
  void validateHostHeader(short port, RequestDts& dts);
  void ValidateContentEncoding(RequestDts& dts);
  void hostHeaderNameCheck(std::string hostName, RequestDts& dts);
  void hostHeaderportCheck(short port, std::string portName, RequestDts& dts);

  void removeNotAscii(std::string& field);

 private:
  void requestChecker(RequestDts& dts);

  void checkRequestLine(RequestDts& dts);
  void checkMethod(RequestDts& dts);
  void checkProtocolVersion(RequestDts& dts);
  void checkRequestUriLimitLength(RequestDts& dts);
  void checkContentLenghWithTransferEncoding(RequestDts& dts);
  void checkHeaderLimitSize(RequestDts& dts);
  void checkBodyLimitLength(RequestDts& dts);
  void checkAllowedMethods(RequestDts& dts);
  void checkCgiMethod(RequestDts& dts);
  void checkTE(RequestDts& dts);
  void checkContentRangeHeader(RequestDts& dts);
  void checkContentType(RequestDts& dts);
  void checkExpectHeader(RequestDts& dts);

 private:
  const std::set<std::string>& _candidateFields;

 public:
  void parseRequest(RequestDts& dts, short port);

  static RequestParser& getInstance();
};
