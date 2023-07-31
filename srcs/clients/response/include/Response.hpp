#pragma once

#include <iostream>

#include "Config.hpp"
#include "IRequest.hpp"
#include "IResponse.hpp"
#include "Status.hpp"
#include "Utils.hpp"

/**
 * @brief Response 클래스는 IResponse 인터페이스를 구현한 클래스이다.
 *
 * @author
 * @date 2023.07.29
 */
class Response : public IResponse {
 public:
  Response();
  virtual ~Response();
  Response(const Response &src);
  Response &operator=(const Response &src);

 private:
  std::string _response;
  bool _responseFlag;
  bool _assembleFlag;
  Status _statusCode;
  std::map<std::string, std::string> _headerFields;
  std::string _body;

  void resetResponse();

  void assembleResponseLine(void);
  void putHeaderFields(void);
  void putBody(void);
  void configureErrorPages(RequestDts &dts);

  void create300Response(RequestDts &dts);
  void create400And500Response(RequestDts &dts);

 public:
  virtual void createExceptionResponse(RequestDts &dts);
  void createEmptyExceptionResponse(RequestDts &dts);
  bool getResponseFlag(void) const;
  Status getStatus(void);
  const std::string &getResponse(void) const;
  const std::string &getBody(void) const;
  std::string &getFieldValue(const std::string &key);
  void assembleResponse(void);

  void eraseHeaderField(const std::string &key);
  void addBody(const std::string &str);

  void setHeaderField(const std::string &key, const std::string &value);
  void setBody(const std::string &str);
  void setStatusCode(Status code);
  void setResponseParsed();
  bool isParsed();

  void setCookie(std::string &session_id);
  void clear(void);
};
