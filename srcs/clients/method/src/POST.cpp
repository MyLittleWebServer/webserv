#include "POST.hpp"

#include <unistd.h>

#include <fstream>

#include "Utils.hpp"

POST::POST(void) {}

POST::~POST(void) {}

void POST::doRequest(RequestDts& dts, IResponse& response) {
#ifndef DEBUG_MSG
  std::cout << " >>>>>>>>>>>>>>> POST\n";
  std::cout << "path: " << *dts.path << "\n";
  std::cout << "body: " << *dts.body << "\n";
  std::cout << "content-type: " << (*dts.headerFields)["content-type"] << "\n";
  std::cout << "content-length: " << (*dts.headerFields)["content-length"]
            << "\n";
#endif
  (void)response;
  this->generateResource(dts);
  response.setStatusCode(E_201_CREATED);
}

void POST::generateResource(RequestDts& dts) {
  this->_contentType = (*dts.headerFields)["content-type"].c_str();
  std::string parsedContent = this->_contentType;
  if (this->_contentType.find(';') != std::string::npos) {
    parsedContent = _contentType.substr(0, _contentType.find(';'));
  }
  if (parsedContent == "application/x-www-form-urlencoded") {
    this->generateUrlEncoded(dts);
  } else if (parsedContent == "multipart/form-data") {
    this->generateMultipart(dts);
  }
}

void POST::generateUrlEncoded(RequestDts& dts) {
  std::cout << "rawBody: " << *dts.body << "\n";
  std::string decodedBody = decodeURL(*dts.body);

  std::cout << "decodedBody: " << decodedBody << "\n";
  if (decodedBody.find("title") == std::string::npos ||
      decodedBody.find("content") == std::string::npos)
    throw(*dts.statusCode = E_400_BAD_REQUEST);

  size_t andPos = decodedBody.find('&');
  size_t equalPos1 = decodedBody.find('=');
  if (andPos == std::string::npos || equalPos1 == std::string::npos) {
    throw(*dts.statusCode = E_400_BAD_REQUEST);
  }
  this->_title = decodedBody.substr(equalPos1 + 1, andPos - equalPos1 - 1);
  decodedBody = decodedBody.substr(andPos + 1);
  size_t equalPos2 = decodedBody.find('=');
  this->_content = decodedBody.substr(equalPos2 + 1);
#ifdef DEBUG_MSG
  std::cout << "title: " << this->_title << "\n";
  std::cout << "content: " << this->_content << "\n";
  std::cout << "path: " << *dts.path << "\n";
#endif
  writeTextBody(dts);
}

void POST::generateMultipart(RequestDts& dts) {
  std::string binBody = (*dts.body).data();

  size_t boundaryEndPos = binBody.find("\r\n");
  if (boundaryEndPos == std::string::npos)
    throw((*dts.statusCode) = E_400_BAD_REQUEST);
  this->_boundary = binBody.substr(0, boundaryEndPos);

  size_t filePos = binBody.find("filename=");
  size_t fileEndPos = binBody.find("\r\n", filePos);
  if (filePos == std::string::npos || fileEndPos == std::string::npos) {
    this->_title = "Invalid File Name";
    this->_content = "Invalid File Source";
    writeTextBody(dts);
    return;
  }
  this->_title = binBody.substr(filePos + 10, fileEndPos - filePos - 11);
  size_t binStart = (*dts.body).find("\r\n\r\n");
  size_t boundary2EndPos = (*dts.body).find(this->_boundary, fileEndPos);
  if (binStart == std::string::npos || boundary2EndPos == std::string::npos)
    throw((*dts.statusCode) = E_400_BAD_REQUEST);

  this->_content.insert(this->_content.end(),
                        (*dts.body).begin() + binStart + 4,
                        (*dts.body).begin() + boundary2EndPos);
  writeBinaryBody(dts);
}

void POST::writeTextBody(RequestDts& dts) {
  if (stat(dts.path->c_str(), &fileinfo) != 0)
    throw((*dts.statusCode) = E_403_FORBIDDEN);
  std::string pathInfo = *dts.path + this->_title + ".txt";
  std::cout << "pathInfo: " << pathInfo << "\n";
  std::ofstream file(pathInfo, std::ios::out);
  file << this->_content << "\n";
  file.close();
}

void POST::writeBinaryBody(RequestDts& dts) {
  if (stat(dts.path->c_str(), &fileinfo) != 0)
    throw((*dts.statusCode) = E_403_FORBIDDEN);
  std::string filename = *dts.path + this->_title;
  std::ofstream file(filename.c_str(), std::ios::binary);
  if (!file.is_open()) throw((*dts.statusCode) = E_500_INTERNAL_SERVER_ERROR);
  file << this->_content;
  if (file.fail()) throw((*dts.statusCode) = E_500_INTERNAL_SERVER_ERROR);
  file.close();
  if (file.fail()) throw((*dts.statusCode) = E_500_INTERNAL_SERVER_ERROR);
}

void POST::createSuccessResponse(IResponse& response) {
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
  bzero(buf, buf_len);
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
