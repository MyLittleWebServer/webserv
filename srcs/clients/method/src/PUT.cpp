#include "PUT.hpp"

#include <fstream>

#include "Utils.hpp"

PUT::PUT(void) {}

PUT::~PUT(void) {}

/**
 * @brief PUT 메소드를 실행합니다.
 *
 * @details
 * PUT 메소드는 요청된 리소스를 생성하거나 수정합니다.
 * 요청된 리소스가 존재하지 않는 경우 생성하고, 존재하는 경우 수정합니다.
 * PUT 메소드는 요청된 리소스의 전체를 업데이트합니다.
 *
 * @param RequestDts HTTP 관련 데이터
 * @param IResponse 응답 객체
 *
 * @author
 * @date 2023.07.30
 */
void PUT::doRequest(RequestDts& dts, IResponse& response) {
  if (*dts.body == "") {
    throw(*dts.statusCode = E_200_OK);
  }
  initUniqueIdandPath(dts);
  struct stat fileInfo;
  if (stat(dts.path->c_str(), &fileInfo) < 0) {
    replaceContent(dts);
    response.setStatusCode(E_201_CREATED);
  } else {
    if (checkBodyContent(dts)) {
      throw(*dts.statusCode = E_204_NO_CONTENT);
    }
    replaceContent(dts);
    response.setStatusCode(E_200_OK);
  }
}

/**
 * @brief PUT 메소드를 실행하기 전, uniqueID와 pathFinder를 초기화합니다.
 *
 * @details
 * uniqueID는 요청된 리소스의 이름을 나타냅니다.
 * pathFinder는 요청된 리소스의 경로를 나타냅니다.
 *
 * @param dts
 *
 * @author
 * @date 2023.07.30
 */
void PUT::initUniqueIdandPath(RequestDts& dts) {
  _uniqueID = "";
  const std::string& dtsPath = *dts.path;
  size_t slashPos = dtsPath.rfind('/');
  _pathFinder = (*dts.path).substr(0, slashPos) + "/";
  if (slashPos != std::string::npos && dtsPath[slashPos + 1] != '\0') {
    _uniqueID = dtsPath.substr(slashPos + 1);
  }
}

/**
 * @brief PUT 메소드를 실행하기 전, body의 내용을 확인합니다.
 *
 * @details
 * body의 내용이 일치하는지 확인합니다.
 * body의 내용이 일치하지 않는 경우 false를 반환합니다.
 *
 * @param dts
 * @return true
 * @return false
 *
 * @author
 * @date 2023.07.30
 */
bool PUT::checkBodyContent(RequestDts& dts) {
  const std::string& contentType = (*dts.headerFields)["content-type"];
  std::string parsedContent;
  bool ret;

  if (contentType.find(';') != std::string::npos) {
    parsedContent = contentType.substr(0, contentType.find(';'));
  } else {
    parsedContent = contentType;
  }
  if (parsedContent == "multipart/form-data") {
    ret = checkForMultipart(dts);
  } else if (parsedContent == "application/x-www-form-urlencoded") {
    ret = checkForUrlEncoded(dts);
  } else {
    ret = checkForTextFile(dts);
  }
  return ret;
}

/**
 * @brief body의 내용이 multipart/form-data인 경우, body의 내용을 확인합니다.
 *
 * @details
 * body의 내용이 일치하는지 확인합니다.
 * body의 내용이 일치하지 않는 경우 false를 반환합니다.
 *
 * @param dts
 * @return true
 * @return false
 *
 * @author
 * @date 2023.07.30
 */
bool PUT::checkForMultipart(RequestDts& dts) {
  std::string cmpBoundary;
  std::string cmpContent;
  std::string binBody = (*dts.body).data();
  size_t boundaryEndPos = binBody.find("\r\n");
  if (boundaryEndPos == std::string::npos)
    throw((*dts.statusCode) = E_400_BAD_REQUEST);
  cmpBoundary = binBody.substr(0, boundaryEndPos);

  size_t filePos = binBody.find("filename=\"");
  size_t fileEndPos = binBody.find('\"', filePos + 11);
  size_t binStart = (*dts.body).find("\r\n\r\n");
  size_t boundary2EndPos = (*dts.body).find(cmpBoundary, fileEndPos);
  if (binStart == std::string::npos || boundary2EndPos == std::string::npos)
    throw((*dts.statusCode) = E_400_BAD_REQUEST);
  cmpContent.insert(cmpContent.end(), (*dts.body).begin() + binStart + 4,
                    (*dts.body).begin() + boundary2EndPos);

  std::ifstream file(dts.path->c_str(), std::ios::binary);
  if (!file.is_open()) {
    throw(*dts.statusCode = E_500_INTERNAL_SERVER_ERROR);
  }
  std::stringstream buf;
  buf << file.rdbuf();
  if (file.fail()) {
    throw(*dts.statusCode = E_500_INTERNAL_SERVER_ERROR);
  }
  file.close();

  if (cmpContent.compare(buf.str()) != 0) {
    return false;
  }
  return true;
}

/**
 * @brief body의 내용이 urlencoded인 경우, body의 내용을 확인합니다.
 *
 * @details
 * body의 내용이 일치하는지 확인합니다.
 * body의 내용이 일치하지 않는 경우 false를 반환합니다.
 *
 * @param dts
 * @return true
 * @return false
 *
 * @author
 * @date 2023.07.30
 */

bool PUT::checkForUrlEncoded(RequestDts& dts) {
  std::ifstream file(dts.path->c_str(), std ::ios::in);
  if (!file.is_open()) {
    throw(*dts.statusCode = E_500_INTERNAL_SERVER_ERROR);
  }
  std::string urlContent;
  std::getline(file, urlContent);
  if (file.fail()) {
    throw(*dts.statusCode = E_500_INTERNAL_SERVER_ERROR);
  }
  file.close();

  std::string compStr = "title=" + _uniqueID + "&" + "content=" + urlContent;
  size_t andPos = (*dts.body).find('&');
  size_t equalPos = (*dts.body).find('=', andPos + 1);
  std::string bodyContent = (*dts.body).substr(equalPos + 1);
  std::string bodyStr = "title=" + _uniqueID + "&" + "content=" + bodyContent;

  if (compStr == bodyStr) {
    return true;
  }
  return false;
}

/**
 * @brief body의 내용이 text(js/JSON/txt) 파일인 경우, body의 내용을 확인합니다.
 *
 * @param dts
 * @return true
 * @return false
 *
 * @author
 * @date 2023.07.30
 */
bool PUT::checkForTextFile(RequestDts& dts) {
  std::ifstream file(dts.path->c_str(), std::ios::in);
  if (!file.is_open()) {
    throw((*dts.statusCode) = E_500_INTERNAL_SERVER_ERROR);
  }
  std::string cmpContent;
  std::string buf;
  while (std::getline(file, buf)) {
    cmpContent += buf;
    if (file.fail()) {
      throw((*dts.statusCode) = E_500_INTERNAL_SERVER_ERROR);
    }
  }
  file.close();

  if (cmpContent == (*dts.body)) {
    return true;
  }
  return false;
}

/**
 * @brief Contnet-type에 따라 body의 내용을 교체합니다.
 *
 * @details
 * content-type에 따라 body의 내용을 교체합니다.
 * 만들어진 파일 이름은 uniqueID로 설정합니다.
 *
 * @param dts
 *
 * @author
 * @date 2023.07.30
 */
void PUT::replaceContent(RequestDts& dts) {
  std::string& dtsBody = *dts.body;
  std::string cmpBoundary;
  std::string parsedContent;

  const std::string& contentType = (*dts.headerFields)["content-type"];
  if (contentType.find(';') != std::string::npos) {
    parsedContent = contentType.substr(0, contentType.find(';'));
  } else {
    parsedContent = contentType;
  }

  if (parsedContent == "multipart/form-data") {
    std::string binBody = dtsBody.data();
    size_t boundaryEndPos = binBody.find("\r\n");
    if (boundaryEndPos == std::string::npos) {
      throw((*dts.statusCode) = E_400_BAD_REQUEST);
    }
    cmpBoundary = binBody.substr(0, boundaryEndPos);

    size_t filePos = binBody.find("filename=\"");
    size_t fileEndPos = binBody.find('\"', filePos + 11);
    size_t binStart = dtsBody.find("\r\n\r\n");
    size_t boundary2EndPos = dtsBody.find(cmpBoundary, fileEndPos);

    if (binStart == std::string::npos || boundary2EndPos == std::string::npos) {
      throw(*dts.statusCode = E_400_BAD_REQUEST);
    }
    _content.insert(_content.end(), dtsBody.begin() + binStart + 4,
                    dtsBody.begin() + boundary2EndPos);
    writeBinaryBody(dts);
  } else if (parsedContent == "application/x-www-form-urlencoded") {
    std::vector<std::string> splitbyAnd = ft_split(dtsBody, '&');
    size_t equalPos = splitbyAnd[1].find('=');
    _content = splitbyAnd[1].substr(equalPos + 1);
    writeTextBody(dts);
  } else {
    _content = dtsBody.data();
    writeTextBody(dts);
  }
}

/**
 * @brief Contnet-type에 따라 body의 내용을 생성합니다.
 *
 * @details
 * content-type에 따라 body의 내용을 생성합니다.
 * 만들어진 파일 이름은 uniqueID로 설정합니다.
 *
 * @param dts
 *
 * @author
 * @date 2023.07.30
 */
void PUT::generateResource(RequestDts& dts) {
  std::string parsedContent;
  const std::string& contentType = (*dts.headerFields)["content-type"];

  if (contentType.find(';') != std::string::npos) {
    parsedContent = contentType.substr(0, contentType.find(';'));
  } else {
    parsedContent = contentType;
  }
  if (parsedContent == "application/x-www-form-urlencoded") {
    generateUrlEncoded(dts);
  } else if (parsedContent == "multipart/form-data") {
    generateMultipart(dts);
  } else {
    writeTextBody(dts);
  }
}

/**
 * @brief application/x-www-form-urlencoded의 body를 파싱합니다.
 *
 * @details
 * body에는 querystring으로 title과 content가 들어옵니다.
 * title과 content가 key로 들어오지 않으면 400 Bad Request를 던집니다.
 * title의 value값이 없으면 uniqueID로 파일명을 생성합니다.
 * mime-type을 text/plain으로 설정하고 writeTextBody를 실행합니다.
 *
 * @param dts
 *
 * @author
 * @date 2023.07.30
 */
void PUT::generateUrlEncoded(RequestDts& dts) {
  std::string decodedBody = decodeURL(*dts.body);

  if (decodedBody.find("title") == std::string::npos ||
      decodedBody.find("content") == std::string::npos) {
    throw(*dts.statusCode = E_400_BAD_REQUEST);
  }

  size_t andPos = decodedBody.find('&');
  size_t equalPos1 = decodedBody.find('=');
  if (andPos == std::string::npos || equalPos1 == std::string::npos) {
    throw(*dts.statusCode = E_400_BAD_REQUEST);
  }
  _title = _uniqueID;
  size_t equalPos2 = decodedBody.find('=', andPos + 1);
  _content = decodedBody.substr(equalPos2 + 1);
  writeTextBody(dts);
}

/**
 * @brief multipart/form-data의 body를 파싱합니다.
 *
 * @details
 * body에는 boundary가 들어옵니다.
 * boundary를 기준으로 파일명과 파일내용을 파싱합니다.
 * filename 없으면 uniqueID로 생성합니다.
 * body에 있는 이진데이터를 writeBinaryBody를 통해 파일에 씁니다.
 * PUT요청은 uri를 통해 파일을 생성하므로, 중복되는 경우 418을 던집니다.
 *
 * @param dts
 *
 * @author
 * @date 2023.07.30
 */
void PUT::generateMultipart(RequestDts& dts) {
  std::string& dtsBody = *dts.body;

  size_t boundaryEndPos = dtsBody.find("\r\n");

  if (boundaryEndPos == std::string::npos) {
    throw((*dts.statusCode) = E_400_BAD_REQUEST);
  }
  std::string boundary = dtsBody.substr(0, boundaryEndPos);

  while (true) {
    std::string binBody = dtsBody.data();
    size_t filePos = binBody.find("filename=\"");
    size_t fileEndPos = binBody.find('\"', filePos + 11);
    if (filePos == std::string::npos || fileEndPos == std::string::npos) {
      _title = _uniqueID;
      _content = "DummySource";
      writeTextBody(dts);
      return;
    }
    _title = _uniqueID;
    size_t binStart = dtsBody.find("\r\n\r\n");
    size_t boundary2EndPos = dtsBody.find(boundary, fileEndPos);
    if (binStart == std::string::npos || boundary2EndPos == std::string::npos) {
      throw(*dts.statusCode = E_400_BAD_REQUEST);
    }
    _content.insert(_content.end(), dtsBody.begin() + binStart + 4,
                    dtsBody.begin() + boundary2EndPos);
    writeBinaryBody(dts);
    size_t isEOFCRLF = dtsBody.find("\r\n", boundary2EndPos);
    std::string isEOF =
        dtsBody.substr(boundary2EndPos, isEOFCRLF - boundary2EndPos);
    if (isEOF == boundary + "--") {
      _title.clear();
      _content.clear();
      dtsBody.clear();
      return;
    }
    _title.clear();
    _content.clear();
    throw(*dts.statusCode = E_418_IM_A_TEAPOT);
  }
}

/**
 * @brief mimeType에 따라 파일을 생성합니다.
 *
 * @details
 * 파일이 존재하는지 확인하고 존재하면 403 Forbidden을 던집니다.
 * path에 /가 없으면 /를 붙여서 uniqueID를 가진 파일을 생성합니다.
 *
 * @param dts
 * @param mimeType
 *
 * @author
 * @date 2023.07.30
 */
void PUT::writeTextBody(RequestDts& dts) {
  std::string filename;

  if (_pathFinder[_pathFinder.length() - 1] != '/') {
    filename = _pathFinder + "/" + _uniqueID;
  } else {
    filename = _pathFinder + _uniqueID;
  }

  std::ofstream file(filename.c_str(), std::ios::out);
  if (!file.is_open()) {
    throw((*dts.statusCode) = E_500_INTERNAL_SERVER_ERROR);
  }
  file << _content;
  if (file.fail()) {
    throw((*dts.statusCode) = E_500_INTERNAL_SERVER_ERROR);
  }
  file.close();
  _title.clear();
  _content.clear();
}

/**
 * @brief 이진데이터를 파일에 씁니다.
 *
 * @details
 * 파일이 존재하는지 확인하고 존재하면 403 Forbidden을 던집니다.
 * path에 /가 없으면 /를 붙여서 uniqueID를 가진 파일을 생성합니다.
 * 이진데이터를 담고있는 _content를 파일에 씁니다.
 *
 * @param dts
 *
 * @author
 * @date 2023.07.30
 */
void PUT::writeBinaryBody(RequestDts& dts) {
  std::string filename;

  if (_pathFinder[_pathFinder.length() - 1] != '/') {
    filename = _pathFinder + "/" + _uniqueID;
  } else {
    filename = _pathFinder + _uniqueID;
  }

  std::ofstream file(filename.c_str(), std::ios::binary);
  if (!file.is_open()) {
    throw((*dts.statusCode) = E_500_INTERNAL_SERVER_ERROR);
  }
  file << _content;
  if (file.fail()) {
    throw((*dts.statusCode) = E_500_INTERNAL_SERVER_ERROR);
  }
  file.close();
}

/**
 * @brief RFC PUT의 맞게 응답 객체를 설정합니다. 응답 객체를 파싱합니다.
 *
 * @param IResponse 응답 객체
 *
 * @author
 * @date 2023.07.30
 */
void PUT::createSuccessResponse(IResponse& response) {
  response.setHeaderField("Location", "/put_test/" + _uniqueID);
  response.setBody("File has been successfully uploaded.\r\npath: /directory/" +
                   _uniqueID + "\r\n");
  response.setHeaderField("Content-Type", "text/plain");
  response.setHeaderField("Content-Length", itos(response.getBody().size()));
  response.assembleResponse();
  response.setResponseParsed();
}

/**
 * @brief URL을 디코딩합니다.
 *
 * @details
 * URL을 디코딩합니다.
 * URL을 디코딩한 결과를 반환합니다.
 *
 * @param encodedString 디코딩할 URL
 * @return std::string 디코딩된 URL
 *
 * @author
 * @date 2023.07.30
 */
std::string PUT::decodeURL(std::string encodedString) {
  std::replace(encodedString.begin(), encodedString.end(), '+', ' ');
  size_t len = encodedString.length();
  int buf_len = 0;
  for (size_t i = 0; i < len; ++i) {
    if (encodedString.at(i) == '%') {
      i += 2;
    }
    ++buf_len;
  }
  char* buf = new char[buf_len];
  std::memset(buf, 0, buf_len);
  char c = 0;
  size_t j = 0;
  for (size_t i = 0; i < len; ++i, ++j) {
    if (encodedString.at(i) == '%') {
      c = 0;
      c += encodedString.at(i + 1) >= 'A' ? 16 * (encodedString.at(i + 1) - 55)
                                          : 16 * (encodedString.at(i + 1) - 48);
      c += encodedString.at(i + 2) >= 'A' ? (encodedString.at(i + 2) - 55)
                                          : (encodedString.at(i + 2) - 48);
      i += 2;
    } else {
      c = encodedString.at(i);
    }
    buf[j] = c;
  }

  std::string decoded_string;
  for (int i = 0; i < buf_len; ++i) {
    decoded_string.push_back(buf[i]);
  }

  delete[] buf;
  return decoded_string;
}
