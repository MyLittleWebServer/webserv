#include "POST.hpp"

#include <unistd.h>

#include <fstream>

#include "Utils.hpp"

POST::POST(void) {}

POST::~POST(void) {}

void POST::doRequest(RequestDts& dts, IResponse& response) {
  // std::map<std::string, std::string>::iterator it =
  // (dts.headerFields->begin()); std::map<std::string, std::string>::iterator
  // end = (dts.headerFields->end()); while (it != end) {
  //   std::cout << "key: " << it->first << "   value: " << it->second << "\n";
  //   it++;
  // }
  this->generateResource(dts, response);
  *dts.statusCode = CREATED;
}

void POST::generateResource(RequestDts& dts, IResponse& response) {
  this->_contentType = (*dts.headerFields)["content-type"].c_str();
  std::string parsedContent = this->_contentType;
  if (this->_contentType.find(';') != std::string::npos) {
    parsedContent = _contentType.substr(0, _contentType.find(';'));
  }
  std::cout << "CONTENT-TYPE: " << parsedContent << "\n";
  if (parsedContent == "application/x-www-form-urlencoded") {
    this->generateUrlEncoded(dts, response);
  } else if (parsedContent == "multipart/form-data") {
    this->generateMultipart(dts, response);
  }
}

void POST::generateUrlEncoded(RequestDts& dts, IResponse& response) {
  std::cout << "rawBody: " << *dts.body << "\n";
  std::string decodedBody = decodeURL(*dts.body);

  std::cout << "decodedBody: " << decodedBody << "\n";
  if (decodedBody.find("title") == std::string::npos ||
      decodedBody.find("content") == std::string::npos)
    throw(*dts.statusCode = BAD_REQUEST);

  size_t andPos = decodedBody.find('&');
  size_t equalPos1 = decodedBody.find('=');
  if (andPos == std::string::npos || equalPos1 == std::string::npos) {
    throw(*dts.statusCode = BAD_REQUEST);
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
  prepareTextBody(dts);

  *dts.statusCode = CREATED;
  createSuccessResponse(response);
}

void POST::generateMultipart(RequestDts& dts, IResponse& response) {
  size_t boundaryPos = this->_contentType.find("boundary");
  if (boundaryPos == std::string::npos) throw(*dts.statusCode = BAD_REQUEST);
  std::string boundaryValue = this->_contentType.substr(boundaryPos + 10);
  if (boundaryValue == "") throw(*dts.statusCode = BAD_REQUEST);

  prepareBinaryBody(dts);

  *dts.statusCode = CREATED;
  createSuccessResponse(response);
}

void POST::prepareTextBody(RequestDts& dts) {
  if (access(dts.path->c_str(), F_OK) < 0) throw((*dts.statusCode) = FORBIDDEN); 
  std::string pathInfo = *dts.path + this->_title + ".txt";
  std::cout << "pathInfo: " << pathInfo << "\n";
  std::ofstream file(pathInfo, std::ios::out);
  if (!file.is_open()) throw((*dts.statusCode) = INTERNAL_SERVER_ERROR);
  file << this->_content << "\n";
  if (file.fail()) throw((*dts.statusCode) = INTERNAL_SERVER_ERROR);
  file.close();
  if (file.fail()) throw((*dts.statusCode) = INTERNAL_SERVER_ERROR);
}

void POST::prepareBinaryBody(RequestDts& dts) {
  if (access(dts.path->c_str(), F_OK) < 0) throw((*dts.statusCode) = FORBIDDEN);
  std::string filename = *dts.path + "tmpTitle";
  std::ofstream file(filename.c_str(), std::ios::binary);
  std::string binBody = (*dts.body).data();
  size_t boundaryPos = binBody.find("boundary");
  
  if (boundaryPos == std::string::npos) throw((*dts.statusCode) = BAD_REQUEST);
  size_t start = binBody.find("\r\n\r\n", boundaryPos, binBody.find("Content-Type"));
  size_t end = binBody.rfind("------WebKitFormBoundary");
  if (start == std::string::npos || end == std::string::npos) 
    throw((*dts.statusCode) = BAD_REQUEST);
  start += 4;
  end -= 2;
  std::string content = binBody.substr(start, end - start).data();
  file << content;
  file.close();
}

void POST::createSuccessResponse(IResponse& response) {
  const std::string& value = response.getFieldValue("Content-Type");
  response.setHeaderField("Content-Type", value + "; charset=UTF-8");
  response.setHeaderField("Date", getCurrentTime());
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
  char *buf = new char[buf_len];
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
