#include "PUT.hpp"

#include <fstream>

#include "Utils.hpp"

PUT::PUT(void) {}

PUT::~PUT(void) {}

void PUT::doRequest(RequestDts& dts, IResponse& response) {
  std::cout << " >>>>>>>>>>>>>>> PUT\n";
  std::cout << "path: " << *dts.path << "\n";
  std::cout << "content-type: " << (*dts.headerFields)["content-type"] << "\n";
  std::cout << "content-length: " << (*dts.headerFields)["content-length"]
            << "\n";
  if (*dts.body == "") throw(*dts.statusCode = E_200_OK);
  initUniqueIdentifier(dts);

  // check if resource already existed
  if (access(dts.path->c_str(), F_OK) == -1) {
    generateResource(dts);  // generate resource when no existed
    response.setStatusCode(E_201_CREATED);
  } else {
    if (checkBodyContent(dts))
      throw(*dts.statusCode = E_204_NO_CONTENT);  // body contents is different
    replaceContent(dts);
    response.setStatusCode(E_200_OK);  // no change occured
  }
}

void PUT::initUniqueIdentifier(RequestDts& dts) {
  _uniqueID = "";
  _uniqueContent = (*dts.body).data();
  std::string tmpPath = *dts.path;
  std::cout << "tmpPath : " << tmpPath << "\n";
  size_t slashPos = tmpPath.rfind('/');
  if (slashPos != std::string::npos && tmpPath[slashPos + 1] != '\0') {
    _uniqueID = tmpPath.substr(slashPos + 1);
    std::cout << "_uniqueID : " << _uniqueID << "\n";
  }
}

bool PUT::checkBodyContent(RequestDts& dts) {
  std::string parsedContent;
  _contentType = (*dts.headerFields)["content-type"].c_str();
  parsedContent = _contentType;
  if (_contentType.find(';') != std::string::npos) {
    parsedContent = _contentType.substr(0, _contentType.find(';'));
  }
  if (parsedContent == "multipart/form-data") {
    checkForMultipart(dts);  // check for memory inside memcmp?
  } else if (parsedContent == "application/x-www-form-urlencoded") {
    checkForUrlEncoded(dts);  // check for urlencode type
  } else {                    // check for plain/text
    checkForPlainText(dts);
    // writeTextBody(dts);
  }
  return false;
}

bool PUT::checkForMultipart(RequestDts& dts) {
  /* get binary current body content*/
  std::string tmpBoundary;
  std::string tmpContent;
  std::string binBody = (*dts.body).data();
  size_t boundaryEndPos = binBody.find("\r\n");
  if (boundaryEndPos == std::string::npos)
    throw((*dts.statusCode) = E_400_BAD_REQUEST);
  tmpBoundary = binBody.substr(0, boundaryEndPos);

  std::string binBody = (*dts.body).data();
  size_t filePos = binBody.find("filename=\"");
  size_t fileEndPos = binBody.find('\"', filePos + 11);
  size_t binStart = (*dts.body).find("\r\n\r\n");
  size_t boundary2EndPos = (*dts.body).find(tmpBoundary, fileEndPos);
  if (binStart == std::string::npos || boundary2EndPos == std::string::npos)
    throw((*dts.statusCode) = E_400_BAD_REQUEST);
  tmpContent.insert(tmpContent.end(), (*dts.body).begin() + binStart + 4,
                    (*dts.body).begin() + boundary2EndPos);

  /* get binary file body content*/
  std::ifstream file(dts.path->c_str(), std::ios::binary);
  std::stringstream buf;
  std::string compBin;
  buf << file.rdbuf();
  compBin += (buf.str());
  file.close();
  if (memcmp(compBin.c_str(), tmpContent.c_str(), sizeof(compBin.c_str())) == 0)
    return true;
  _alterContent = tmpContent.data();
}

bool PUT::checkForUrlEncoded(RequestDts& dts) {
  std::string urlTitle = dts.path->c_str();  // ?not sure how to get filename?
  std::string urlContent;
  std::string buf;
  std::ifstream file(dts.path->c_str(), std::ios::in);
  if (!file.is_open()) throw((*dts.statusCode) = E_500_INTERNAL_SERVER_ERROR);
  std::getline(file, buf);
  urlContent += buf;
  // urlContent += '\n';

  std::string compStr = "title=" + urlTitle + "&" + "content=" + urlContent;
  if (compStr == *(dts.body)) return true;
}

bool PUT::checkForPlainText(RequestDts& dts) {
  std::ifstream file(dts.path->c_str(), std::ios::in);
  if (!file.is_open()) throw((*dts.statusCode) = E_500_INTERNAL_SERVER_ERROR);
  std::string tmpContent;
  std::string buf;
  while (std::getline(file, buf)) {
    tmpContent += buf;
    tmpContent += '\n';
    if (file.fail()) throw((*dts.statusCode) = E_500_INTERNAL_SERVER_ERROR);
  }
  file.close();
  if (file.fail()) throw((*dts.statusCode) = E_500_INTERNAL_SERVER_ERROR);
  if (tmpContent == (*dts.body)) return true;
  _alterContent = tmpContent;
  return;
}

void PUT::replaceContent(RequestDts& dts) {}

// below functions are from POST

void PUT::generateResource(RequestDts& dts) {
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

void PUT::generateUrlEncoded(RequestDts& dts) {
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

void PUT::generateMultipart(RequestDts& dts) {
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

void PUT::writeTextBody(RequestDts& dts, std::string mimeType) {
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

void PUT::writeBinaryBody(RequestDts& dts) {
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

void PUT::createSuccessResponse(IResponse& response) {
  response.setHeaderField("Location", "/post_body/" + _title);
  response.setBody("File has been successfully uploaded.\r\npath: /directory/" +
                   _title + "\r\n");
  response.setHeaderField("Content-Type", "text/plain");
  response.setHeaderField("Content-Length", itos(response.getBody().size()));
  response.assembleResponse();
  response.setResponseParsed();
}

std::string PUT::decodeURL(std::string encoded_string) {
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

std::string PUT::makeRandomFileName(RequestDts& dts) {
  std::string charset =
      "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
  std::string result;
  std::srand(std::time(0));

  for (int i = 0; i < 8; ++i)
    result.push_back(charset[std::rand() % charset.size()]);

  if (access(result.c_str(), F_OK) == 0)
    throw((*dts.statusCode) = E_409_CONFLICT);

  return result;
}
/*
POST : create only
PUT : update if not existed then crated

1. 클라이언트는 PUT 요청을 보낼 때 리소스의 고유 식별자를 요청 URL 또는 경로에
포함시킵니다. 이 식별자는 해당 리소스를 유일하게 식별할 수 있는 정보로
사용됩니다.

2. 서버는 받은 PUT 요청의 URL 또는 경로에서 리소스의 고유 식별자를 추출합니다.

3. 서버는 추출한 고유 식별자를 사용하여 해당 리소스가 이미 존재하는지 여부를
확인합니다.

4-1. 리소스가 존재하는 경우: 서버는 해당 리소스를 업데이트하고 PUT 요청의
데이터를 기존 리소스에 적용합니다.

4-2. 리소스가 존재하지 않는 경우: 서버는 새로운 리소스를 생성하고 고유 식별자를
가진 새로운 리소스를 생성합니다. 만약 리소스를 생성하기 위해 요청 URL에 새로운
식별자를 포함시켰다면, 해당 식별자를 새 리소스의 고유 식별자로 사용합니다.

5. 적절한 응답: 서버는 적절한 상태 코드와 응답 본문을 클라이언트로 반환하여
작업의 성공 여부를 알려줍니다.
*/
