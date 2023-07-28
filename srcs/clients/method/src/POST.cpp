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

void POST::doRequest(RequestDts& dts, IResponse& response) {
#ifndef DEBUG_MSG
  std::cout << " >> POST\n";
  std::cout << "path: " << *dts.path << "\n";
  std::cout << "original path: " << *dts.originalPath << "\n";
  std::cout << "content-type: " << (*dts.headerFields)["content-type"] << "\n";
  std::cout << "content-length: " << (*dts.headerFields)["content-length"]
            << "\n";
#endif
  if (*dts.body == "") throw(*dts.statusCode = E_204_NO_CONTENT);
  handlePath(dts, response);
}

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
      throw(*dts.statusCode = E_415_UNSUPPORTED_MEDIA_TYPE);
    }
  }
}

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
#ifdef DEBUG_MSG
  std::cout << "title: " << _title << "\n";
  std::cout << "content: " << _content << "\n";
  std::cout << "path: " << *dts.path << "\n";
#endif
  writeTextBody(dts, "txt");
}

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
  file << _content << "\n";
  if (file.fail()) throw((*dts.statusCode) = E_500_INTERNAL_SERVER_ERROR);
  file.close();
  if (file.fail()) throw((*dts.statusCode) = E_500_INTERNAL_SERVER_ERROR);
  _title.clear();
  _content.clear();
}

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
}

void POST::createSuccessResponse(IResponse& response) {
  response.setHeaderField("Location", "/post_body/" + _title);
  response.setBody("File has been successfully uploaded.\r\npath: /directory/" +
                   _title + "\r\n");
  response.setHeaderField("Content-Type", "text/plain");
  response.setHeaderField("Content-Length", itos(response.getBody().size()));
  response.assembleResponse();
  response.setResponseParsed();
}

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

void POST::handlePath(RequestDts& dts, IResponse& response) {
  if (getSpecificEndpoint(dts, response))
    return;
  else {
    generateResource(dts);
    response.setStatusCode(E_201_CREATED);
  }
}

/**
 * @brief 특정 URL의 엔드포인트를 POST하는 함수
 *
 * @details
 *
 *
 * @param dts
 * @param response
 * @return true : true를 리턴하면 해당 POST요청을 더이상 진행하지 않음.
 * @return false : false를 리턴하면 해당 함수를 종료.
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
#ifdef DEBUG_MSG
    std::cout << "session not found " << e.what() << std::endl;
#endif
    throw(*dts.statusCode = E_401_UNAUTHORIZED);
  }
  return false;
}

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

void POST::submit(RequestDts& dts, Session& session) {
  SessionData& sessionData =
      session.getSessionData((*dts.cookieMap)["session_id"]);
  sessionData.setData("data", *dts.body);
}
