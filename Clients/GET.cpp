#include "GET.hpp"

#include <unistd.h>

#include <fstream>

GET::GET() {}
GET::GET(std::string &request) : AMethod(request) {}
GET::~GET() {}

void GET::getFilePath(void) {
  char cwd[1024];

  std::memset(cwd, 0, sizeof(cwd));
  if (getcwd(cwd, sizeof(cwd)) == NULL)
    throw std::runtime_error("getcwd error");
  this->_path = cwd + this->_path;
}

void GET::parseRequestMessage(void) {
  this->splitLinesByCRLF();
  this->parseRequestLine();
}

void GET::createResponseMessage(void) {
  this->getFilePath();
  std::ifstream file(this->_path, std::ios::in);
  if (file.is_open() == false) throw std::runtime_error("file open error");
  this->_response += "HTTP/1.1 200 OK\r\n";
  this->_response += "Content-Type: text/html; charset=UTF-8\r\n";
  this->_response += "\r\n";

  std::string buff;
  while (std::getline(file, buff)) this->_response += buff + "\r\n";
  this->_responseFlag = true;
  file.close();
}