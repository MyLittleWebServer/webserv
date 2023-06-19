#include "GET.hpp"

GET::GET() {}
GET::GET(std::string &request) : AMethod(request) {}
GET::~GET() {}

void GET::parseRequest(void) {
  this->splitLinesByCRLF();
  this->parseRequestLine();
}

void GET::createResponse(void) {
  char cwd[1024];

  std::memset(cwd, 0, sizeof(cwd));
  if (getcwd(cwd, sizeof(cwd)) == NULL)
    throw std::runtime_error("getcwd error");
  this->_path = cwd + this->_path;
  std::ifstream file(this->_path, std::ios::in);
  this->_response += "HTTP/1.1 200 OK\r\n";
  this->_response += "Content-Type: text/html\r\n";
  this->_response += "\r\n";

  if (file.is_open()) {
    std::string line;

    while (std::getline(file, line)) {
      this->_response += line + "\r\n";
    }
    this->_responseFlag = true;
    file.close();  // 파일 닫기
  };
}