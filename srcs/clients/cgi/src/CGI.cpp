#include "CGI.hpp"

CGI::CGI() {
  _cgiResult = "";
  _excuteFlag = false;
  _finishFlag = false;
  initEnv();
}

CGI::CGI(IRequest* request, uintptr_t client_fd) {
  _client_fd = client_fd;
  _cgiResult = "";
  _excuteFlag = false;
  _finishFlag = false;
  this->_request = request;
  initEnv();
}

CGI::~CGI() {}

CGI::CGI(const CGI& copy) { *this = copy; }

CGI& CGI::operator=(const CGI& copy) {
  if (this != &copy) {
    this->_request = copy._request;
    this->_client_fd = copy._client_fd;
    this->_cgiResult = copy._cgiResult;
    this->_excuteFlag = copy._excuteFlag;
    this->_finishFlag = copy._finishFlag;
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
  std::string scritp_filename = "SCRIPT_FILENAME=" + _request->getPath();

  if (_request->getMethod() == "POST") _env.push_back(content_length.c_str());
  if (_request->getMethod() == "GET") _env.push_back(query_string.c_str());
  _env.push_back(content_type.c_str());
  _env.push_back(http_user_agent.c_str());
  _env.push_back(path_info.c_str());
  _env.push_back(remote_addr.c_str());
  _env.push_back(scritp_filename.c_str());
  _env.push_back(script_name.c_str());
  _env.push_back(server_software.c_str());
  _env.push_back(request_method.c_str());
  _env.push_back(server_name.c_str());
  _env.push_back(server_software.c_str());
}

void CGI::execute() {
  if (!_excuteFlag) excuteCgi();
}

const std::string& CGI::getCgiResult() {
  if (!_finishFlag) waitAndRead();
  if (!_finishFlag) throw new ExceptionThrower::CGINotFinishedException();
  return this->_cgiResult;
}

void CGI::setPipeNonblock() {
  setFcntl(_in_pipe[0]);
  setFcntl(_in_pipe[1]);
  setFcntl(_out_pipe[0]);
  setFcntl(_out_pipe[1]);
}

void CGI::setFcntl(int fd) {
  int flags = fcntl(fd, F_GETFL, 0);
  fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

void CGI::excuteCgi() {
  // char** env;

  // env = getEnv();
  _excuteFlag = true;

  if (pipe(_in_pipe) < 0 || pipe(_out_pipe) < 0) {
    throw new ExceptionThrower::CGIPipeException();
  }
  setPipeNonblock();
  Kqueue::addEvent(_in_pipe[1], EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0,
                   (void*)this, FD_CGI);
  // write(in_pipe[1], getBody().c_str(), getBody().size());

  close(_in_pipe[1]);

  _pid = fork();

  if (_pid == -1) {
    // throw new ExceptionThrower::CGIForkException();
  } else if (!_pid) {
    close(_out_pipe[0]);
    dup2(_in_pipe[0], STDIN_FILENO);
    dup2(_out_pipe[1], STDOUT_FILENO);
    close(_in_pipe[0]);
    close(_out_pipe[1]);
    execve(_request->getPath().c_str(), NULL, const_cast<char**>(_env.data()));
    // throw new ExceptionThrower::CGIExcuteException();
  }
  waitAndRead();
}

void CGI::waitAndRead() {
  if (_finishFlag) return;
  waitChild();
  if (!_finishFlag) return;
  close(_in_pipe[0]);
  close(_out_pipe[1]);
  readChild();
  close(_out_pipe[0]);
}

void CGI::waitChild() {
  if (waitpid(_pid, NULL, WNOHANG)) _finishFlag = true;
}

void CGI::readChild() {
  char buffer[1024];
  memset(buffer, 0, 1024);
  while (read(_out_pipe[0], buffer, 1024 - 1) > 0) {
    _cgiResult += buffer;
    memset(buffer, 0, 1024);
  }
}
