#include "PUT.hpp"

#include <fstream>

#include "Utils.hpp"

PUT::PUT(void) {}

PUT::~PUT(void) {}

void PUT::doRequest(RequestDts& dts, IResponse& response) {
#ifdef DEBUG_MSG
  std::cout << " >> PUT\n";
  std::cout << "path: " << *dts.path << "\n";
  std::cout << "content-type: " << (*dts.headerFields)["content-type"] << "\n";
  std::cout << "content-length: " << (*dts.headerFields)["content-length"]
            << "\n";
#endif
  if (*dts.body == "") throw(*dts.statusCode = E_200_OK);
  initUniqueIdandPath(dts);

  if (stat(dts.path->c_str(), &fileinfo) < 0) {
    generateResource(dts);
    response.setStatusCode(E_201_CREATED);
  } else {
    if (checkBodyContent(dts))
      throw(*dts.statusCode = E_204_NO_CONTENT);
    replaceContent(dts);
    response.setStatusCode(E_200_OK);
  }
}

void PUT::initUniqueIdandPath(RequestDts& dts) {
  _uniqueID = "";
  std::string uniquePath = *dts.path;
  size_t slashPos = uniquePath.rfind('/');
  _pathFinder = (*dts.path).substr(0, slashPos) + "/";
  if (slashPos != std::string::npos && uniquePath[slashPos + 1] != '\0') {
    _uniqueID = uniquePath.substr(slashPos + 1);
  }
}

bool PUT::checkBodyContent(RequestDts& dts) {
  std::string parsedContent;
  bool ret;
  _contentType = (*dts.headerFields)["content-type"].c_str();
  parsedContent = _contentType;
  if (_contentType.find(';') != std::string::npos) {
    parsedContent = _contentType.substr(0, _contentType.find(';'));
  }
  if (parsedContent == "multipart/form-data") {
    ret = checkForMultipart(dts);
  } else if (parsedContent == "application/x-www-form-urlencoded") {
    ret = checkForUrlEncoded(dts);
  } else {                          
    ret = checkForPlainText(dts);
  }
  return ret;
}

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

  /* get binary file body content */
  std::ifstream file(dts.path->c_str(), std::ios::binary);
  std::stringstream buf;
  std::string cmpBin;
  buf << file.rdbuf();
  cmpBin += (buf.str());
  file.close();
  if (cmpContent.compare(cmpBin) != 0) {
      return false;
  }
  return true;
}

bool PUT::checkForUrlEncoded(RequestDts& dts) {
  std::string urlTitle = _uniqueID;
  std::string urlContent;
  std::string buf;
  std::ifstream file(dts.path->c_str(), std ::ios::in);
  if (!file.is_open()) throw((*dts.statusCode) = E_500_INTERNAL_SERVER_ERROR);
  std::getline(file, buf);
  urlContent += buf;

  std::string compStr = "title=" + urlTitle + "&" + "content=" + urlContent;  
  size_t andPos = (*dts.body).find('&');
  size_t equalPos = (*dts.body).find('=', andPos + 1);
  std::string bodyContent = (*dts.body).substr(equalPos + 1);
  std::string bodyStr = "title=" + urlTitle + "&" + "content=" + bodyContent;

  if (compStr == bodyStr) return true;
  return false;
}

bool PUT::checkForPlainText(RequestDts& dts) {
#ifdef DEBUG_MSG
  std::cout << "dts.path : " << *dts.path << "\n";
#endif
  std::ifstream file(dts.path->c_str(), std::ios::in);
  if (!file.is_open()) throw((*dts.statusCode) = E_500_INTERNAL_SERVER_ERROR);
  std::string cmpContent;
  std::string buf;
  while (std::getline(file, buf)) {
    cmpContent += buf;
    if (file.fail()) throw((*dts.statusCode) = E_500_INTERNAL_SERVER_ERROR);
  }
  file.close();
  if (cmpContent == (*dts.body)) return true;
  return false;
}

void PUT::replaceContent(RequestDts& dts) {
  std::string cmpBoundary;
  std::string binBody = (*dts.body).data();
  std::string parsedContent;
  _contentType = (*dts.headerFields)["content-type"].c_str();
  parsedContent = _contentType;
  if (_contentType.find(';') != std::string::npos) {
    parsedContent = _contentType.substr(0, _contentType.find(';'));
  }

  if (parsedContent == "multipart/form-data") {
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
    _content.insert(_content.end(), (*dts.body).begin() + binStart + 4,
                    (*dts.body).begin() + boundary2EndPos);

    writeBinaryBody(dts);
  } else if (parsedContent == "application/x-www-form-urlencoded") {
    std::vector<std::string> splitbyAnd = ft_split(*dts.body, '&');
    size_t equalPos = splitbyAnd[1].find('=');
    _content = splitbyAnd[1].substr(equalPos + 1);
    writeTextBody(dts);
  }
  else {
    _content = (*dts.body).data();
    writeTextBody(dts);
  }
}

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
    writeTextBody(dts);
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
  _title = _uniqueID;
  size_t equalPos2 = decodedBody.find('=', andPos + 1);
  _content = decodedBody.substr(equalPos2 + 1);
#ifdef DEBUG_MSG
  std::cout << "title: " << _title << "\n";
  std::cout << "content: " << _content << "\n";
  std::cout << "path: " << *dts.path << "\n";
#endif
  writeTextBody(dts);
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
      _title = _uniqueID;
      _content = "DummySource";
      writeTextBody(dts);
      return;
    }
    _title = _uniqueID;
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
    _title.clear();
    _content.clear();
    throw(*dts.statusCode = E_418_IM_A_TEAPOT);
  }
}

void PUT::writeTextBody(RequestDts& dts) {
  std::string filename;
  if (_pathFinder[_pathFinder.length() - 1] != '/')
    filename = _pathFinder + "/" + _uniqueID;
  else
    filename = _pathFinder + _uniqueID;
  std::ofstream file(filename.c_str(), std::ios::out);
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
  if (_pathFinder[_pathFinder.length() - 1] != '/')
    filename = _pathFinder + "/" + _uniqueID;
  else
    filename = _pathFinder + _uniqueID;
  std::ofstream file(filename.c_str(), std::ios::binary);
  if (!file.is_open()) throw((*dts.statusCode) = E_500_INTERNAL_SERVER_ERROR);
  file << _content;
  if (file.fail()) throw((*dts.statusCode) = E_500_INTERNAL_SERVER_ERROR);
  file.close();
  if (file.fail()) throw((*dts.statusCode) = E_500_INTERNAL_SERVER_ERROR);
}

void PUT::createSuccessResponse(IResponse& response) {
  response.setHeaderField("Location", "/put_test/" + _uniqueID);
  response.setBody("File has been successfully uploaded.\r\npath: /directory/" +
                   _uniqueID + "\r\n");
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
