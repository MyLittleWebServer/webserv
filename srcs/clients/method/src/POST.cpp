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

// void POST::generateFile(RequestDts& dts) {
//   if (access(dts.path->c_str(), F_OK) < 0) throw((*dts.statusCode) =
//   FORBIDDEN); std::ofstream file(dts.path->c_str(), std::ios::out); if
//   (!file.is_open()) throw((*dts.statusCode) = INTERNAL_SERVER_ERROR);
//   std::list<std::string>::const_iterator it = dts.linesBuffer->begin();
//   std::list<std::string>::const_iterator end = dts.linesBuffer->end();
//   while (it != end) {
//     file << *it++;
//     if (file.fail()) throw((*dts.statusCode) = INTERNAL_SERVER_ERROR);
//   }
//   file.close();
//   if (file.fail()) throw((*dts.statusCode) = INTERNAL_SERVER_ERROR);
// }

void POST::generateResource(RequestDts& dts, IResponse& response) {
  this->_contentType = (*dts.headerFields)["content-type"].c_str();
  std::string parsedContent = this->_contentType;
  if (this->_contentType.find(';') != std::string::npos) {
    parsedContent = _contentType.substr(0, _contentType.find(';'));
  }
  // std::cout << "CONTENT-TYPE" << parsedContent << "\n";
  if (parsedContent == "application/x-www-form-urlencoded") {
    std::cout << "sibal1\n";
    this->generateUrlEncoded(dts, response);
  } else if (parsedContent == "multipart/form-data") {
    std::cout << "sibal2\n";
    this->generateMultipart(dts, response);
  }
}

void POST::generateUrlEncoded(RequestDts& dts, IResponse& response) {
  std::string decodedBody = decodeURL(this->_body);

  if (decodedBody.find("title") == std::string::npos ||
      decodedBody.find("content") == std::string::npos)
    throw(*dts.statusCode = BAD_REQUEST);

  size_t andPos = decodedBody.find('&');
  size_t equalPos1 = decodedBody.find('=');
  if (andPos == std::string::npos || equalPos1 == std::string::npos) {
    throw(*dts.statusCode = BAD_REQUEST);
  }
  this->_title = decodedBody.substr(equalPos1, andPos - equalPos1);
  decodedBody = decodedBody.substr(andPos);
  size_t equalPos2 = decodedBody.find('=');
  this->_content = decodedBody.substr(equalPos2);

  if (access(this->_path.c_str(), F_OK) < 0) throw(*dts.statusCode = FORBIDDEN);

  prepareTextBody(this->_path);

  *dts.statusCode = CREATED;
  createSuccessResponse(response);
}

void POST::generateMultipart(RequestDts& dts, IResponse& response) {
  size_t boundaryPos = this->_contentType.find("boundary");
  if (boundaryPos == std::string::npos) throw(*dts.statusCode = BAD_REQUEST);
  std::string boundaryValue = this->_contentType.substr(boundaryPos + 10);
  if (boundaryValue == "") throw(*dts.statusCode = BAD_REQUEST);

  prepareBinaryBody(this->_path);

  *dts.statusCode = CREATED;
  createSuccessResponse(response);
}

void POST::prepareTextBody(const std::string& path) {
  std::ofstream file(path.c_str(), std::ios::out);
  std::string buff;
  file.close();
}

void POST::prepareBinaryBody(const std::string& filename) {
  std::ofstream file(filename.c_str(), std::ios::binary);
  std::stringstream buffer;
  buffer << file.rdbuf();
  this->_body = buffer.str();
  file.close();
}

void POST::createSuccessResponse(IResponse& response) {
  const std::string& value = response.getFieldValue("Content-Type");
  response.setHeaderField("Content-Type", value + "; charset=UTF-8");
  response.setHeaderField("Date", getCurrentTime());
  response.assembleResponse();
  response.setResponseParsed();
}

std::string POST::createHTML(std::string const& head) {
  std::string html = "<html><body>";
  html += "<a href=\"" + head + "\">" + head + "</a><br>";
  html += "</body></html>";
  return html;
}

std::string POST::decodeURL(std::string const& encoded_string) {
  int buf_len = 0;
  std::string decoded_string = encoded_string;
  std::replace(decoded_string.begin(), decoded_string.end(), '+', ' ');
  size_t len = decoded_string.length();

  for (size_t i = 0; i < len; ++i) {
    if (encoded_string.at(i) == '%') i += 2;
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
  for (int i = 0; i < buf_len; ++i) decoded_string.push_back(buf[i]);
  delete[] buf;
  return decoded_string;
}
