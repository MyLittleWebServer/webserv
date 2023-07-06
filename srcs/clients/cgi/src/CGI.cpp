#include "CGI.hpp"

CGI::CGI() {
  _cgiResult = "";
  _excuteFlag = false;
  _finishFlag = false;
  this->request = request;
}

CGI::CGI(IRequest* request) {
  _cgiResult = "";
  _excuteFlag = false;
  _finishFlag = false;
  this->request = request;
}

CGI::~CGI() {}

CGI::CGI(const CGI& copy) { *this = copy; }

CGI& CGI::operator=(const CGI& copy) {
  if (this != &copy) {
    this->_cgiResult = copy._cgiResult;
    this->_excuteFlag = copy._excuteFlag;
    this->_finishFlag = copy._finishFlag;
  }
  return *this;
}

void CGI::execute() {
  if (!_excuteFlag) excuteCgi();
}

const std::string& CGI::getCgiResult() {
  if (!_finishFlag) waitAndRead();
  if (!_finishFlag) throw new ExceptionThrower::CGINotFinishedException();
  return this->_cgiResult;
}

void CGI::excuteCgi() {
  char** env;

  // env = getEnv();
  _excuteFlag = true;

  if (pipe(_in_pipe) < 0 || pipe(_out_pipe) < 0) {
    throw new ExceptionThrower::CGIPipeException();
  }

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
    // execve(getName().c_str(), NULL, env);
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
