#include "POST.hpp"

#include <fcntl.h>

#include <cstdlib>
#include <cstring>
#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>

#include "Utils.hpp"

POST::POST(void) {}

POST::~POST(void) {}

/**
 * @brief POST 메소드를 실행합니다.
 *
 * @details
 * body에 내용이 없으면 204 No Content를 던집니다.
 * handlePath 메소드를 통해 특정 URL의 엔드포인트에서 POST요청을 합니다.
 *
 * @param RequestDts
 * @param IResponse
 *
 * @author
 * @date 2023.07.29
 */
void POST::doRequest(RequestDts& dts, IResponse& response) {
  if (*dts.body == "") throw(*dts.statusCode = E_204_NO_CONTENT);
  handlePath(dts, response);
}

/**
 * @brief content-type에 따라 리소스를 생성합니다.
 *
 * @details
 * content-type이 application/x-www-form-urlencoded이면 generateUrlEncoded를
 * content-type이 multipart/form-data이면 generateMultipart를
 * content-type이 그 외의 경우에는 mime-type에 따른 writeTextBody를 실행합니다.
 *
 * @param RequestDts
 *
 * @author
 * @date 2023.07.29
 */
void POST::generateResource(RequestDts& dts) {
  std::string parsedContent;
  _contentType = (*dts.headerFields)["content-type"].c_str();
  parsedContent = _contentType;
  if (_contentType.find(';') != std::string::npos) {
    parsedContent = _contentType.substr(0, _contentType.find(';'));
  }
  if (parsedContent == "application/x-www-form-urlencoded") {
    generateUrlEncoded(dts);
  } else if (parsedContent == "multipart/form-data") {
    generateMultipart(dts);
  } else {
    std::string mimeType = "bin";
    MimeTypesConfig& mime = dynamic_cast<MimeTypesConfig&>(
        Config::getInstance().getMimeTypesConfig());
    try {
      std::string mimeType = mime.getVariable(parsedContent);
      _content = (*dts.body);
      _title = makeRandomFileName(dts);
      writeTextBody(dts, mimeType);
    } catch (ExceptionThrower::InvalidConfigException& e) {
      _content = (*dts.body);
      _title = makeRandomFileName(dts);
      writeTextBody(dts, mimeType);
    }
  }
}

/**
 * @brief application/x-www-form-urlencoded의 body를 파싱합니다.
 *
 * @details
 * body에는 querystring으로 title과 content가 들어옵니다.
 * title과 content가 key로 들어오지 않으면 400 Bad Request를 던집니다.
 * title의 value값이 없으면 랜덤한 파일명을 생성합니다.
 * mime-type을 text/plain으로 설정하고 writeTextBody를 실행합니다.
 *
 * @param dts
 *
 * @author
 * @date 2023.07.29
 */
void POST::generateUrlEncoded(RequestDts& dts) {
  std::string decodedBody = decodeURL(*dts.body);

  if (decodedBody.find("title") == std::string::npos ||
      decodedBody.find("content") == std::string::npos)
    throw(*dts.statusCode = E_400_BAD_REQUEST);

  size_t andPos = decodedBody.find('&');
  size_t equalPos1 = decodedBody.find('=');
  if (andPos == std::string::npos || equalPos1 == std::string::npos) {
    throw(*dts.statusCode = E_400_BAD_REQUEST);
  }
  _title = decodedBody.substr(equalPos1 + 1, andPos - equalPos1 - 1);
  if (_title == "") {
    _title = makeRandomFileName(dts);
  }
  size_t equalPos2 = decodedBody.find('=', andPos + 1);
  _content = decodedBody.substr(equalPos2 + 1);
  writeTextBody(dts, "txt");
}

/**
 * @brief multipart/form-data의 body를 파싱합니다.
 *
 * @details
 * body에는 boundary가 들어옵니다.
 * boundary를 기준으로 파일명과 파일내용을 파싱합니다.
 * filename 없으면 랜덤한 파일명을 생성합니다.
 * body에 있는 이진데이터를 writeBinaryBody를 통해 파일에 씁니다.
 * boundary 마지막에 --가 있으면 멀티파트의 파일생성을 종료합니다.
 *
 * @param dts
 *
 * @author
 * @date 2023.07.29
 */
void POST::generateMultipart(RequestDts& dts) {
  std::string binBody = (*dts.body).data();

  size_t boundaryEndPos = binBody.find("\r\n");
  if (boundaryEndPos == std::string::npos)
    throw((*dts.statusCode) = E_400_BAD_REQUEST);
  _boundary = binBody.substr(0, boundaryEndPos);

  while (true) {
    std::string binBody = (*dts.body).data();
    size_t filePos = binBody.find("filename=\"");
    size_t fileEndPos = binBody.find('\"', filePos + 11);
    if (filePos == std::string::npos || fileEndPos == std::string::npos) {
      _title = makeRandomFileName(dts);
      _content = "DummySource";
      writeTextBody(dts, "txt");
      return;
    }
    _title = binBody.substr(filePos + 10, fileEndPos - filePos - 10);
    if (_title == "") {
      _title = makeRandomFileName(dts);
    }
    size_t binStart = (*dts.body).find("\r\n\r\n");
    size_t boundary2EndPos = (*dts.body).find(_boundary, fileEndPos);
    if (binStart == std::string::npos || boundary2EndPos == std::string::npos)
      throw((*dts.statusCode) = E_400_BAD_REQUEST);
    _content.insert(_content.end(), (*dts.body).begin() + binStart + 4,
                    (*dts.body).begin() + boundary2EndPos);
    writeBinaryBody(dts);
    size_t isEOFCRLF = (*dts.body).find("\r\n", boundary2EndPos);
    std::string isEOF =
        (*dts.body).substr(boundary2EndPos, isEOFCRLF - boundary2EndPos);
    if (isEOF == _boundary + "--") {
      _title.clear();
      _content.clear();
      (*dts.body).clear();
      return;
    }
    (*dts.body) = (*dts.body).substr(boundary2EndPos);
    _title.clear();
    _content.clear();
  }
}

/**
 * @brief mimeType에 따라 파일을 생성합니다.
 *
 * @details
 * 파일이 존재하는지 확인하고 존재하면 403 Forbidden을 던집니다.
 * path에 /가 없으면 /를 붙여서 파일명을 생성합니다.
 * mimeType에 따라 파일을 생성하고 파일에 내용을 씁니다.
 *
 * @param dts
 * @param mimeType
 *
 * @author
 * @date 2023.07.29
 */
void POST::writeTextBody(RequestDts& dts, std::string mimeType) {
  std::string filename;
  if (stat(dts.path->c_str(), &fileinfo) != 0)
    throw((*dts.statusCode) = E_403_FORBIDDEN);
  if ((*dts.path)[dts.path->length() - 1] != '/')
    filename = *dts.path + "/" + _title + "." + mimeType;
  else
    filename = *dts.path + _title + "." + mimeType;
  std::ofstream file(filename, std::ios::out);
  if (!file.is_open()) throw((*dts.statusCode) = E_500_INTERNAL_SERVER_ERROR);
  file << _content;
  if (file.fail()) throw((*dts.statusCode) = E_500_INTERNAL_SERVER_ERROR);
  file.close();
  if (file.fail()) throw((*dts.statusCode) = E_500_INTERNAL_SERVER_ERROR);
  if ((*dts.originalPath)[dts.originalPath->length() - 1] != '/')
    _location = *dts.originalPath + "/" + _title + "." + mimeType;
  else
    _location = *dts.originalPath + _title + "." + mimeType;
  _title.clear();
  _content.clear();
}

/**
 * @brief 이진데이터를 파일에 씁니다.
 *
 * @details
 * 파일이 존재하는지 확인하고 존재하면 403 Forbidden을 던집니다.
 * path에 /가 없으면 /를 붙여서 파일명을 생성합니다.
 * 이진데이터를 담고있는 _content를 파일에 씁니다.
 *
 * @param dts
 *
 * @author
 * @date 2023.07.29
 */
void POST::writeBinaryBody(RequestDts& dts) {
  std::string filename;
  if (stat(dts.path->c_str(), &fileinfo) != 0)
    throw((*dts.statusCode) = E_403_FORBIDDEN);
  if ((*dts.path)[dts.path->length() - 1] != '/')
    filename = *dts.path + "/" + _title;
  else
    filename = *dts.path + _title;
  std::ofstream file(filename.c_str(), std::ios::binary);
  if (!file.is_open()) throw((*dts.statusCode) = E_500_INTERNAL_SERVER_ERROR);
  file << _content;
  if (file.fail()) throw((*dts.statusCode) = E_500_INTERNAL_SERVER_ERROR);
  file.close();
  if (file.fail()) throw((*dts.statusCode) = E_500_INTERNAL_SERVER_ERROR);

  if ((*dts.originalPath)[dts.originalPath->length() - 1] != '/')
    _location = *dts.originalPath + "/" + _title;
  else
    _location = *dts.originalPath + _title;
}

/**
 * @brief RFC POST의 맞게 응답 객체를 설정합니다. 응답 객체를 파싱합니다.
 *
 * @param response
 *
 * @author
 * @date 2023.07.29
 */
void POST::createSuccessResponse(IResponse& response) {
  response.setHeaderField("Location", _location);
  response.setBody("File has been successfully uploaded.\r\npath: /directory/" +
                   _title + "\r\n");
  response.setHeaderField("Content-Type", "text/plain");
  response.setHeaderField("Content-Length", itos(response.getBody().size()));
  response.assembleResponse();
  response.setResponseParsed();
}

/**
 * @brief URL 디코딩을 합니다.
 *
 * @details
 * 16진수로 인코딩된 문자열을 디코딩합니다.
 *
 * @param encoded_string
 * @return std::string
 *
 * @author
 * @date 2023.07.29
 */
std::string POST::decodeURL(std::string encoded_string) {
  std::replace(encoded_string.begin(), encoded_string.end(), '+', ' ');
  size_t len = encoded_string.length();
  int buf_len = 0;
  for (size_t i = 0; i < len; ++i) {
    if (encoded_string.at(i) == '%') {
      i += 2;
    }
    ++buf_len;
  }
  char* buf = new char[buf_len];
  std::memset(buf, 0, buf_len);
  char c = 0;
  size_t j = 0;
  for (size_t i = 0; i < len; ++i, ++j) {
    if (encoded_string.at(i) == '%') {
      c = 0;
      c += encoded_string.at(i + 1) >= 'A'
               ? 16 * (encoded_string.at(i + 1) - 55)
               : 16 * (encoded_string.at(i + 1) - 48);
      c += encoded_string.at(i + 2) >= 'A' ? (encoded_string.at(i + 2) - 55)
                                           : (encoded_string.at(i + 2) - 48);
      i += 2;
    } else {
      c = encoded_string.at(i);
    }
    buf[j] = c;
  }
  std::string decoded_string;
  for (int i = 0; i < buf_len; ++i) decoded_string.push_back(buf[i]);

  delete[] buf;
  return decoded_string;
}

std::string POST::makeRandomFileName(RequestDts& dts) {
  std::string result = generateRandomString();

  if (access(result.c_str(), F_OK) == 0)
    throw((*dts.statusCode) = E_409_CONFLICT);

  return result;
}

/**
 * @brief 특정 URL의 엔드포인트와 세션을 체크하고 POST하는 함수
 *
 * @details
 * 엔드포인트와 세션의 체크를 하고 해당하는 엔드포인트가 없거나 세션이 없으면
 * 리소스를 생성합니다. 리소스를 생성하면 201 Created를 던집니다.
 *
 * @param RequestDts
 * @param IResponse
 *
 * @author
 * @date 2023.07.29
 */
void POST::handlePath(RequestDts& dts, IResponse& response) {
  if (getSpecificEndpoint(dts, response))
    return;
  else {
    generateResource(dts);
    response.setStatusCode(E_201_CREATED);
  }
}

/**
 * @brief 세션체크를 하고 특정 URL의 엔드포인트를 실행합니다.
 *
 * @param RequestDts
 * @param IResponse
 *
 * @return true : true를 리턴하면 해당 POST요청을 더이상 진행하지 않음.
 * @return false : false를 리턴하면 해당 함수를 종료.
 *
 * @author
 * @date 2023.07.29
 */
bool POST::getSpecificEndpoint(RequestDts& dts, IResponse& response) {
  if (*dts.is_session == false) return false;
  Session& session = Session::getInstance();
  const std::string& originalPath = *dts.originalPath;
  try {
    if (originalPath == "/login") {
      login(dts, response, session);
    } else if (originalPath == "/enter") {
      enter(dts, response, session);
    } else if (originalPath == "/session") {
      submit(dts, session);
      return true;
    }
  } catch (ExceptionThrower::SessionDataNotFound& e) {
    throw(*dts.statusCode = E_401_UNAUTHORIZED);
  }
  return false;
}

/**
 * @brief 로그인과 동시에 세션id를 생성하고 처리합니다.
 *
 * @details
 * 세션 id를 생성하고 발급한 세션아이디로 응답에다 쿠키를 설정해줍니다.
 * 세션 데이터에 패스워드를 제외한 로그인 정보를 저장합니다.
 * 로그인이 완료되면 302 Found를 던집니다.
 *
 * @param RequestDts
 * @param IResponse
 * @param Session
 *
 * @author
 * @date 2023.07.29
 */
void POST::login(RequestDts& dts, IResponse& response, Session& session) {
  std::string session_id = session.createSession(getTimeOfDay() + 60);

  response.setCookie(session_id);
  SessionData& sessionData = session.getSessionData(session_id);

  std::vector<std::string> body = ft_split(*dts.body, "&");
  std::vector<std::string>::const_iterator it = body.begin();
  for (; it < body.end(); ++it) {
    std::vector<std::string> keyValue = ft_split(*it, '=');
    if (!keyValue.empty() && keyValue[0] != "password")
      sessionData.setData(keyValue[0], keyValue[1]);
  }

  *dts.path = "";
  throw(*dts.statusCode = E_302_FOUND);
}

/**
 * @brief 페이지 입장과 동시에 세션 데이터를 생성하고 세션 정보를 저장합니다.
 *
 * @details
 * 세션 데이터에 로그인 정보를 저장합니다.
 * 로그인이 완료되면 302 Found를 던집니다.
 *
 * @param RequestDts
 * @param IResponse
 * @param Session
 *
 * @author
 * @date 2023.07.29
 */
void POST::enter(RequestDts& dts, IResponse& response, Session& session) {
  std::string session_id = session.createSession(getTimeOfDay() + 300);

  response.setCookie(session_id);
  SessionData& sessionData = session.getSessionData(session_id);

  std::vector<std::string> body = ft_split(*dts.body, "&");
  std::vector<std::string>::const_iterator it = body.begin();
  for (; it < body.end(); ++it) {
    std::vector<std::string> keyValue = ft_split(*it, '=');
    if (!keyValue.empty()) sessionData.setData(keyValue[0], keyValue[1]);
  }

  *dts.path = "/";
  throw(*dts.statusCode = E_302_FOUND);
}

/**
 * @brief 세션 데이터 저장
 *
 * @details
 * 세션 데이터에 로그인 정보를 저장합니다.
 *
 * @param RequestDts
 * @param Session
 *
 * @author
 * @date 2023.07.29
 *
 */
void POST::submit(RequestDts& dts, Session& session) {
  SessionData& sessionData =
      session.getSessionData((*dts.cookieMap)["session_id"]);
  sessionData.setData("data", *dts.body);
}
