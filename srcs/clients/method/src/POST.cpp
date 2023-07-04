#include "POST.hpp"

#include <unistd.h>

#include <fstream>

#include "Utils.hpp"

POST::POST(void) {}

POST::POST(std::string& request)
    : AMethod(request), _contentType("text/plain") {}

POST::~POST(void) {}

void POST::doRequest() {
  std::istringstream ss(this->_headerFields["content-length"]);
  ss >> this->_body;
  if (this->_body.size() == 0)
    throw(this->_statusCode = BAD_REQUEST);

  else if (this->_body.size() > _matchedLocation->getLimitClientBodySize())
    throw(this->_statusCode = REQUEST_ENTITY_TOO_LARGE);
  this->generateResource();
}

void POST::generateResource() {
  if (_contentType == "application/x-www-form-urlencoded")
    this->generateUrlEncoded();  // username=john_doe&password=secret123
  else if (_contentType.substr(0, 20) == "multipart/form-data" &&
           _contentType.find(";") != std::string::npos)
    this->generateMultipart();
}

void POST::generateUrlEncoded(void) {
  std::string decodedBody = decodeURL(this->_body);

  if (decodedBody.find("title") == std::string::npos ||
      decodedBody.find("content") == std::string::npos)
    throw(this->_statusCode = BAD_REQUEST);

  size_t andPos = decodedBody.find('&');
  size_t equalPos1 = decodedBody.find('=');
  if (andPos == std::string::npos || equalPos1 == std::string::npos) {
    throw(this->_statusCode = BAD_REQUEST);
  }
  this->_title = decodedBody.substr(equalPos1, andPos - equalPos1);
  decodedBody = decodedBody.substr(andPos);
  size_t equalPos2 = decodedBody.find('=');
  this->_content = decodedBody.substr(equalPos2);

  if (access(this->_path.c_str(), F_OK) < 0)
    throw(this->_statusCode = FORBIDDEN);

  prepareTextBody(this->_path);

  this->_statusCode = CREATED;
  createSuccessResponse();
}

void POST::generateMultipart(void) {
  size_t boundaryPos = this->_contentType.find("boundary");
  if (boundaryPos == std::string::npos) throw(this->_statusCode = BAD_REQUEST);
  std::string boundaryValue = this->_contentType.substr(boundaryPos + 10);
  if (boundaryValue == "") throw(this->_statusCode = BAD_REQUEST);

  size_t dispositionPos = _body.find("content-disposition");
  size_t typePos = _body.find("content-type");
  if (dispositionPos == std::string::npos || typePos == std::string::npos)
    throw(this->_statusCode = BAD_REQUEST);

  std::string disposition =
      _body.substr(dispositionPos + 20, typePos - (dispositionPos + 20));
  if (disposition == "") throw(this->_statusCode = BAD_REQUEST);

  size_t equalPos = disposition.find('=');
  size_t semicolonPos = disposition.find(';');
  if (equalPos == std::string::npos || semicolonPos == std::string::npos)
    throw(this->_statusCode = BAD_REQUEST);

  this->_disposName =
      disposition.substr(equalPos + 1, semicolonPos - (equalPos + 1));
  this->_disposFilename = disposition.substr(semicolonPos + 11);
  if (this->_disposName == "" || this->_disposFilename == "")
    throw(this->_statusCode = BAD_REQUEST);

  this->_type = _body.substr(typePos + 14);
  if (this->_type == "") throw(this->_statusCode = BAD_REQUEST);
  if (this->_type != "image/png" && this->_type != "image/jpeg")
    throw(this->_statusCode = UNSUPPORTED_MEDIA_TYPE);

  prepareBinaryBody(this->_disposFilename);

  this->_statusCode = CREATED;
  createSuccessResponse();
}

void POST::prepareTextBody(const std::string& path) {
  std::ifstream file(path.c_str(), std::ios::in);
  std::string buff;
  while (std::getline(file, buff)) this->_body += buff + "\r\n";
  file.close();
}

void POST::prepareBinaryBody(const std::string& filename) {
  std::ifstream file(filename.c_str(), std::ios::binary);
  std::stringstream buffer;
  buffer << file.rdbuf();
  this->_body = buffer.str();
  file.close();
}

void POST::createSuccessResponse(void) {
  assembleResponseLine();
  this->_response += getCurrentTime();
  this->_response += "\r\n";
  this->_response += "Content-Type: text/html; charset=UTF-8\r\n";
  this->_response += "Content-Length: ";
  this->_response += itos(this->_body.size());
  this->_response += "\r\n";
  this->createHTML(this->_title);
  std::cout << this->_response << "\n";
  this->_responseFlag = true;
}

void POST::createDisposSuccessResponse(void) {
  assembleResponseLine();
  this->_response += getCurrentTime();
  this->_response += "\r\n";
  this->_response += "Content-Type: text/html; charset=UTF-8\r\n";
  this->_response += "Content-Length: ";
  this->_response += itos(this->_body.size());
  this->_response += "\r\n";
  this->createHTML(this->_disposFilename);
  std::cout << this->_response << "\n";
  this->_responseFlag = true;
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
