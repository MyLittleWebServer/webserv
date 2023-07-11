#include "CGI.hpp"

CGI::CGI() {
  _cgiResult = "";
  _executeFlag = false;
  _waitFinishFlag = false;
  _cgiFinishFlag = false;
  initEnv();
}

CGI::CGI(IRequest* request, IResponse* response, uintptr_t client_fd,
         void* client_info) {
  _client_fd = client_fd;
  _cgiResult = "";
  _executeFlag = false;
  _waitFinishFlag = false;
  _cgiFinishFlag = false;
  _request = request;
  _response = response;
  _body = _request->getBody();
  _client_info = client_info;
  initEnv();
}

CGI::~CGI() {}

CGI::CGI(const CGI& copy) { *this = copy; }

CGI& CGI::operator=(const CGI& copy) {
  if (this != &copy) {
    this->_request = copy._request;
    this->_response = copy._response;
    this->_client_fd = copy._client_fd;
    this->_cgiResult = copy._cgiResult;
    this->_body = copy._body;
    this->_executeFlag = copy._executeFlag;
    this->_waitFinishFlag = copy._waitFinishFlag;
    this->_cgiFinishFlag = copy._cgiFinishFlag;
    initEnv();
  }
  return *this;
}

void CGI::initEnv() {
  _env.clear();
  std::string content_type =
      "CONTENT_TYPE=" + _request->getHeaderField("CONTENT-TYPE");
  std::string content_length =
      "CONTENT_LENGTH=" + std::to_string(_request->getContentLength());
  std::string http_user_agent =
      "HTTP_USER_AGENT=" + _request->getHeaderField("USER-AGENT");
  std::string server_name =
      "SERVER_=NAME" + _request->getMatchedServer()->getServerName();
  std::string script_name =
      "SCRIPT_NAME=" +
      _request->getPath().substr(_request->getPath().find_last_of('/') + 1);
  std::string server_software = "SERVER_SOFTWARE=HTTP/1.1";
  std::string path_info = "PATH_INFO=" + _request->getPath();
  std::string query_string = "QUERY_STRING=" + _request->getQueryString();
  std::string remote_addr = "REMOTE_ADDR=" + _request->getHeaderField("HOST");
  std::string request_method = "REQUEST_METHOD=" + _request->getMethod();
  std::string script_filename = "SCRIPT_FILENAME=" + _request->getPath();

  if (_request->getMethod() == "POST") _env.push_back(content_length.c_str());
  if (_request->getMethod() == "GET") _env.push_back(query_string.c_str());
  _env.push_back(content_type.c_str());
  _env.push_back(http_user_agent.c_str());
  _env.push_back(path_info.c_str());
  _env.push_back(remote_addr.c_str());
  _env.push_back(script_filename.c_str());
  _env.push_back(script_name.c_str());
  _env.push_back(server_software.c_str());
  _env.push_back(request_method.c_str());
  _env.push_back(server_name.c_str());
  _env.push_back(server_software.c_str());
}

void CGI::generateErrorResponse(Status status) {
  (void)status;
  _cgiFinishFlag = true;
  // _response->setStatusCode(status);
  // _response.createErrorResponse();
  Kqueue::enableEvent(_client_fd, EVFILT_WRITE, _client_info);
};

void CGI::generateResponse() {
  _cgiFinishFlag = true;
  // 첫줄이 200 ok 출력되었을 때에만 response를 생성한다.
  // generateErrorResponse(400);
  // _response->setStatusCode(OK);
  // _response->setBody(_cgiResult);
  // _response->createResponse();
  Kqueue::enableEvent(_client_fd, EVFILT_WRITE, _client_info);
}

void CGI::setFcntl(int fd) {
  int flags = fcntl(fd, F_GETFL, 0);
  fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

void CGI::setPipeNonblock() {
  setFcntl(_in_pipe[0]);
  setFcntl(_in_pipe[1]);
  setFcntl(_out_pipe[0]);
  setFcntl(_out_pipe[1]);
}

void CGI::execute() {
  _executeFlag = true;
  if (access(_request->getPath().c_str(), R_OK) == -1) {
    throw ExceptionThrower::FileAcccessFailedException();
  }
  if (pipe(_in_pipe) < 0) {
    throw ExceptionThrower::CGIPipeException();
  }
  if (pipe(_out_pipe) < 0) {
    close(_in_pipe[0]);
    close(_in_pipe[1]);
    throw ExceptionThrower::CGIPipeException();
  }
  setPipeNonblock();
  Kqueue::setFdSet(_in_pipe[1], FD_CGI);
  Kqueue::addEvent(_in_pipe[1], EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0,
                   static_cast<void*>(this));
}

void CGI::executeCGI() {
  try {
    if (!_executeFlag) execute();
  } catch (ExceptionThrower::CGIPipeException& e) {
    generateErrorResponse(INTERNAL_SERVER_ERROR);
  } catch (ExceptionThrower::FileAcccessFailedException& e) {
    generateErrorResponse(NOT_FOUND);
  }
}

void CGI::makeChild() {
  Kqueue::deleteFdSet(_in_pipe[1], FD_CGI);
  Kqueue::deleteEvent(_in_pipe[1], EVFILT_WRITE);
  close(_in_pipe[1]);
  _pid = fork();
  if (_pid == -1) {
    close(_out_pipe[0]);
    close(_out_pipe[1]);
    close(_in_pipe[0]);
    generateErrorResponse(INTERNAL_SERVER_ERROR);
  } else if (!_pid) {
    dup2(_in_pipe[0], STDIN_FILENO);
    dup2(_out_pipe[1], STDOUT_FILENO);
    close(_in_pipe[0]);
    close(_out_pipe[0]);
    close(_out_pipe[1]);
    execve(_request->getPath().c_str(), NULL, const_cast<char**>(_env.data()));
  }
  Kqueue::setFdSet(_out_pipe[0], FD_CGI);
  Kqueue::addEvent(_out_pipe[0], EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0,
                   static_cast<void*>(this));
}

void CGI::writeCGI() {
  if (_request->getMethod() == "POST") {
    ssize_t ret = write(_in_pipe[1], _body.c_str(), _body.size());
    if (ret != static_cast<ssize_t>(_body.size())) {
      if (ret == -1) {
        Kqueue::deleteFdSet(_in_pipe[1], FD_CGI);
        Kqueue::deleteEvent(_in_pipe[1], EVFILT_WRITE);
        close(_out_pipe[0]);
        close(_out_pipe[1]);
        close(_in_pipe[0]);
        close(_in_pipe[1]);
        generateErrorResponse(INTERNAL_SERVER_ERROR);
        return;
      };
      _body = _body.substr(ret);
      return;
    }
  }
  makeChild();
}

void CGI::waitChild() {
  if (_waitFinishFlag) return;
  pid_t result = waitpid(_pid, NULL, WNOHANG);
  switch (result) {
    case 0:
      break;
    case -1: {
      generateErrorResponse(INTERNAL_SERVER_ERROR);
      close(_in_pipe[0]);
      close(_out_pipe[1]);
      Kqueue::deleteFdSet(_out_pipe[0], FD_CGI);
      Kqueue::deleteEvent(_out_pipe[0], EVFILT_READ);
      close(_out_pipe[0]);
      break;
    }
    default:
      _waitFinishFlag = true;
      break;
  }
}

bool CGI::readChildFinish() {
  char buffer[1024];
  ssize_t readSize;
  while (true) {
    memset(buffer, 0, 1024);
    readSize = read(_out_pipe[0], buffer, 1024 - 1);
    if (readSize == 0) return true;
    if (readSize == -1) return false;
    _cgiResult += buffer;
  }
}

void CGI::waitAndReadCGI() {
  if (_cgiFinishFlag) return;
  waitChild();
  if (!_waitFinishFlag) return;
  close(_in_pipe[0]);
  close(_out_pipe[1]);
  if (!readChildFinish()) return;
  Kqueue::deleteFdSet(_out_pipe[0], FD_CGI);
  Kqueue::deleteEvent(_out_pipe[0], EVFILT_READ);
  close(_out_pipe[0]);
  generateResponse();
}

bool CGI::isCgiFinish() { return _cgiFinishFlag; }

const std::string& CGI::getCgiResult() {
  if (!_cgiFinishFlag) throw ExceptionThrower::CGINotFinishedException();
  return this->_cgiResult;
}
