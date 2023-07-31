#include "CGI.hpp"

#include <signal.h>

CGI::CGI() {
  _lastSentPos = 0;
  _client_fd = 0;
  _cgi_status = CGI_START;
  _cgiResult = "";

  _in_pipe[0] = 0;
  _in_pipe[1] = 0;
  _out_pipe[0] = 0;
  _out_pipe[1] = 0;
  _pid = 0;

  _write_event = false;
  _read_event = false;
  _wait_event = false;

  initEnv();
}

CGI::CGI(IRequest* request, IResponse* response, uintptr_t client_fd,
         void* client_info) {
  _client_fd = client_fd;
  _cgi_status = CGI_START;
  _cgiResult = "";
  _request = request;
  _response = response;
  _body = _request->getBody();
  _client_info = client_info;

  _in_pipe[0] = 0;
  _in_pipe[1] = 0;
  _out_pipe[0] = 0;
  _out_pipe[1] = 0;
  _pid = 0;
  initEnv();
}

void CGI::clearChild() {
  if (_pid > 0) {
    kill(_pid, SIGKILL);
    _pid = 0;
  }
}

void CGI::clearEvent() {
  if (_write_event) {
    if (_in_pipe[1]) {
      Kqueue::deleteFdSet(_in_pipe[1], FD_CGI);
      Kqueue::deleteEvent(_in_pipe[1], EVFILT_WRITE, static_cast<void*>(this));
      _write_event = false;
    }
  }
  if (_wait_event) {
    if (_in_pipe[0]) {
      Kqueue::deleteFdSet(_in_pipe[0], FD_CGI);
      Kqueue::deleteEvent(_in_pipe[0], EVFILT_WRITE, static_cast<void*>(this));
      _wait_event = false;
    }
  }
  if (_read_event) {
    if (_out_pipe[0]) {
      Kqueue::deleteFdSet(_out_pipe[0], FD_CGI);
      Kqueue::deleteEvent(_out_pipe[0], EVFILT_READ, static_cast<void*>(this));
      _read_event = false;
    }
  }
}

void CGI::closePipe(int& fd) {
  if (fd > 0) {
    close(fd);
    fd = 0;
  }
}

void CGI::clearPipe() {
  closePipe(_in_pipe[0]);
  closePipe(_in_pipe[1]);
  closePipe(_out_pipe[0]);
  closePipe(_out_pipe[1]);
}

void CGI::clear() {
  clearChild();
  clearEvent();
  clearPipe();
}

CGI::~CGI() { clear(); }

CGI::CGI(const CGI& copy) { *this = copy; }

CGI& CGI::operator=(const CGI& copy) {
  if (this != &copy) {
    _cgi_status = copy._cgi_status;
    _request = copy._request;
    _response = copy._response;
    _client_fd = copy._client_fd;
    _cgiResult = copy._cgiResult;
    _body = copy._body;
    initEnv();
  }
  return *this;
}

void CGI::initEnv() {
  _env.clear();
  _content_type = "CONTENT_TYPE=" + _request->getHeaderField("CONTENT-TYPE");
  _content_length =
      "CONTENT_LENGTH=" + std::to_string(_request->getContentLength());
  _http_user_agent =
      "HTTP_USER_AGENT=" + _request->getHeaderField("USER-AGENT");
  _server_name = "SERVER_=NAME" + _request->getMatchedServer()->getServerName();
  _server_software = "SERVER_PROTOCOL=HTTP/1.1";
  _path_info = "PATH_INFO=" + _request->getPath();
  _query_string = "QUERY_STRING=" + _request->getQueryString();
  _remote_addr = "REMOTE_ADDR=" + _request->getHeaderField("HOST");
  _request_method = "REQUEST_METHOD=" + _request->getMethod();
  _script_filename = "SCRIPT_FILENAME=" + _request->getPath();
  _cookie = "HTTP_COOKIE=" + _request->getHeaderField("COOKIE");

  if (_request->getMethod() == "POST") _env.push_back(_content_length.c_str());
  if (_request->getMethod() == "GET") _env.push_back(_query_string.c_str());
  if (!_request->getHeaderField("X-Secret-Header-For-Test").empty()) {
    _x_header = "HTTP_X_SECRET_HEADER_FOR_TEST=" +
                _request->getHeaderField("X-Secret-Header-For-Test");
    _env.push_back(_x_header.c_str());
  }
  _env.push_back(_content_type.c_str());
  _env.push_back(_http_user_agent.c_str());
  _env.push_back(_path_info.c_str());
  _env.push_back(_remote_addr.c_str());
  _env.push_back(_script_filename.c_str());
  _env.push_back(_request_method.c_str());
  _env.push_back(_server_name.c_str());
  _env.push_back(_server_software.c_str());
  _env.push_back(_cookie.c_str());
  _env.push_back(NULL);
}

void CGI::generateErrorResponse(Status status) {
  _cgi_status = CGI_END;
  _response->setStatusCode(status);
  _response->assembleResponse();
  _response->setResponseParsed();
  clear();
  Kqueue::enableEvent(_client_fd, EVFILT_WRITE, _client_info);
};

void CGI::generateResponse() {
  _cgi_status = CGI_END;
  size_t ret = _cgiResult.find("Status:");
  if (ret != 0) {
    generateErrorResponse(E_500_INTERNAL_SERVER_ERROR);
    return;
  }
  ret = _cgiResult.find("Status: 200 OK");
  if (ret != 0) {
    generateErrorResponse(E_400_BAD_REQUEST);
    return;
  }
  ssize_t start = _cgiResult.find("\r\n");
  ret = _cgiResult.find("\r\n\r\n", start + 2);
  // std::string header = _cgiResult.substr(start + 2, ret - start);
  // std::string body = _cgiResult.substr(ret + 4);

  size_t headerPos = start + 2;
  size_t headerLength = ret - headerPos;
  size_t bodyPos = ret + 4;
  size_t totalSize = _cgiResult.size();
  size_t bodyLength = totalSize - bodyPos;
  size_t lineEnd;
  size_t colonPos;

  _response->clear();
  _response->setStatusCode(E_200_OK);
  while (true) {
      // ret = header.find("\r\n");
      lineEnd = _cgiResult.find("\r\n", headerPos);
      if (lineEnd == std::string::npos) break;
      // std::string line = header.substr(0, ret);
      colonPos = _cgiResult.find(":", headerPos);
      if (colonPos == std::string::npos) break;
      _response->setHeaderField(
          _cgiResult.substr(headerPos, colonPos - headerPos),
          _cgiResult.substr(colonPos + 1, lineEnd - colonPos - 1));
      // std::string key = line.substr(0, colon);
      // std::string value = line.substr(colon + 1);
      //_response->setHeaderField(key, value);
      headerPos = lineEnd + 2;
      headerLength -= (lineEnd - headerPos);
      // header = header.substr(ret + 2);
  }
  _response->setBody(_cgiResult.substr(bodyPos, bodyLength));
  _response->setHeaderField("Content-Length", itos(bodyLength));
  _response->assembleResponse();
  clear();
  Kqueue::enableEvent(_client_fd, EVFILT_WRITE, _client_info);
}

void CGI::setFcntl(int fd) { fcntl(fd, F_SETFL, O_NONBLOCK); }

void CGI::setPipeNonblock() {
  setFcntl(_in_pipe[0]);
  setFcntl(_in_pipe[1]);
  setFcntl(_out_pipe[0]);
  setFcntl(_out_pipe[1]);
}

void CGI::execute() {
  if (access(_request->getPath().c_str(), R_OK) == -1) {
    throw ExceptionThrower::FileAccessFailedException();
  }
  if (pipe(_in_pipe) < 0) {
    throw ExceptionThrower::CGIPipeException();
  }
  if (pipe(_out_pipe) < 0) {
    throw ExceptionThrower::CGIPipeException();
  }
  setPipeNonblock();
  if (_request->getMethod() == "POST" && _body.size() > 0) {
    Kqueue::setFdSet(_in_pipe[1], FD_CGI);
    Kqueue::addEvent(_in_pipe[1], EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0,
                     static_cast<void*>(this));
    _write_event = true;
  } else {
    closePipe(_in_pipe[1]);
    Kqueue::setFdSet(_in_pipe[0], FD_CGI);
    Kqueue::addEvent(_in_pipe[0], EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0,
                     static_cast<void*>(this));
    _wait_event = true;
  }
  _cgi_status = CGI_WRITE;
  makeChild();
}

void CGI::executeCGI() {
  try {
    if (_cgi_status == CGI_START) execute();
  } catch (ExceptionThrower::CGIPipeException& e) {
    generateErrorResponse(E_500_INTERNAL_SERVER_ERROR);
  } catch (ExceptionThrower::FileAccessFailedException& e) {
    generateErrorResponse(E_404_NOT_FOUND);
  } catch (ExceptionThrower::CGIBodyTooLongException& e) {
    generateErrorResponse(E_413_REQUEST_ENTITY_TOO_LARGE);
  }
}

void CGI::makeChild() {
  _pid = fork();
  if (_pid == -1) {
    generateErrorResponse(E_500_INTERNAL_SERVER_ERROR);
  } else if (!_pid) {
    dup2(_in_pipe[0], STDIN_FILENO);
    dup2(_out_pipe[1], STDOUT_FILENO);
    close(_in_pipe[0]);
    close(_out_pipe[0]);
    close(_out_pipe[1]);
    execve(_request->getPath().c_str(), NULL, const_cast<char**>(_env.data()));
    std::cout << "cgi: execve failed" << std::endl;
    throw(4.2);
  }
  Kqueue::setFdSet(_out_pipe[0], FD_CGI);
  Kqueue::addEvent(_out_pipe[0], EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0,
                   static_cast<void*>(this));
  _read_event = true;
  _cgi_status = CGI_WAIT_CHILD;
}

void CGI::writeCGI() {
  if (_in_pipe[1] == 0) {
    waitChild();
    if (_cgi_status != CGI_RECEIVING) return;
    Kqueue::deleteFdSet(_in_pipe[0], FD_CGI);
    Kqueue::deleteEvent(_in_pipe[0], EVFILT_WRITE, static_cast<void*>(this));
    _wait_event = false;
    closePipe(_in_pipe[0]);
    closePipe(_out_pipe[1]);
    return;
  }
  if (_request->getMethod() == "POST") {
    ssize_t ret = write(_in_pipe[1], _body.c_str() + _lastSentPos,
                        _body.size() - _lastSentPos);
    if (static_cast<size_t>(ret) != _body.size() - _lastSentPos) {
      if (ret == -1) {
        generateErrorResponse(E_500_INTERNAL_SERVER_ERROR);
        return;
      };
      _lastSentPos += ret;
      return;
    }
    // if (_request->getMethod() == "POST") {
    //   ssize_t ret = write(_in_pipe[1], _body.c_str(), _body.size());
    //   if (ret != static_cast<ssize_t>(_body.size())) {
    //     if (ret == -1) {
    //       generateErrorResponse(E_500_INTERNAL_SERVER_ERROR);
    //       return;
    //     };
    //     _body = _body.substr(ret);
    //     return;
    //   }
  }
  Kqueue::deleteFdSet(_in_pipe[1], FD_CGI);
  Kqueue::deleteEvent(_in_pipe[1], EVFILT_WRITE, static_cast<void*>(this));
  _write_event = false;
  closePipe(_in_pipe[1]);
  Kqueue::setFdSet(_in_pipe[0], FD_CGI);
  Kqueue::addEvent(_in_pipe[0], EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0,
                   static_cast<void*>(this));
  _wait_event = true;
}

void CGI::waitChild() {
  if (_cgi_status != CGI_WAIT_CHILD) return;
  pid_t result = waitpid(_pid, NULL, WNOHANG);
  switch (result) {
    case 0:
      break;
    case -1: {
      generateErrorResponse(E_500_INTERNAL_SERVER_ERROR);
      break;
    }
    default:
      _pid = 0;
      _cgi_status = CGI_RECEIVING;
      break;
  }
}

bool CGI::readChildFinish() {
  char buffer[1024];
  ssize_t readSize;
  while (true) {
    std::memset(buffer, 0, 1024);
    readSize = read(_out_pipe[0], buffer, 1024 - 1);
    if (readSize == 0) return true;
    if (readSize == -1) return false;
    _cgiResult += buffer;
  }
}

void CGI::waitAndReadCGI() {
  if (!readChildFinish()) return;
  Kqueue::deleteFdSet(_out_pipe[0], FD_CGI);
  Kqueue::deleteEvent(_out_pipe[0], EVFILT_READ, static_cast<void*>(this));
  _read_event = false;
  closePipe(_out_pipe[0]);
  generateResponse();
}

bool CGI::isCgiFinish() { return _cgi_status == CGI_END; }

const std::string& CGI::getCgiResult() {
  if (_cgi_status != CGI_END) throw ExceptionThrower::CGINotFinishedException();
  return _cgiResult;
}
