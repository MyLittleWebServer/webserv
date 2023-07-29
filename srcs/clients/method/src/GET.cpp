
#include "GET.hpp"

#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

#include <cstdlib>
#include <fstream>
#include <sstream>
#include <vector>

#include "ExceptionThrower.hpp"
#include "Kqueue.hpp"
#include "Session.hpp"
#include "Utils.hpp"

GET::GET() : IMethod() {}
GET::~GET() {}

void GET::doRequest(RequestDts& dts, IResponse& response) {
  handlePath(dts, response);

  if (response.getBody().empty()) {
    *dts.statusCode = E_204_NO_CONTENT;
  }
}

void GET::handlePath(RequestDts& dts, IResponse& response) {
  if (getSpecificEndpoint(dts, response))
    return;
  else
    getPublicEndpoint(dts, response);
}

void GET::getPublicEndpoint(RequestDts& dts, IResponse& response) {
  std::string path = *dts.path;
  std::string pathIndex;
  std::string matchedIndex;
  std::string autoindex;

  if ((*dts.matchedLocation)->getIndex() != "") {
    matchedIndex = (*dts.matchedLocation)->getIndex();
    pathIndex = path + (path[path.size() - 1] == '/' ? matchedIndex
                                                     : "/" + matchedIndex);
    autoindex = (*dts.matchedLocation)->getAutoindex();
  }

  std::cout << "path::" << path << std::endl;
  std::cout << "pathIndex::" << pathIndex << std::endl;
  std::cout << "autoindex::" << autoindex << std::endl;

  if (checkFile(path)) {
    std::cout << "checkFile" << std::endl;
    *dts.statusCode = E_200_OK;
    prepareBody(path, response);
  } else if (checkDirectory(path)) {
    if (checkIndexFile(pathIndex)) {
      std::cout << "check index file" << std::endl;
      *dts.statusCode = E_200_OK;
      prepareBody(pathIndex, response);
    } else if (checkAutoIndex(pathIndex, autoindex)) {
      std::cout << "auto" << std::endl;
      *dts.statusCode = E_200_OK;
      prepareFileList(path, dts, response);
    } else {
      throw(*dts.statusCode = E_404_NOT_FOUND);
    }
  } else {
    throw(*dts.statusCode = E_404_NOT_FOUND);
  }

  // if (access(dts.path->c_str(), R_OK) == 0 &&
  //     (*dts.path)[dts.path->size() - 1] != '/') {
  //   *dts.statusCode = E_200_OK;
  //   prepareBody(*dts.path, response);
  // } else if (access(pathIndex.c_str(), R_OK) == 0 &&
  //            pathIndex[pathIndex.size() - 1] != '/') {
  //   *dts.statusCode = E_200_OK;
  //   // return fileHandler(pathIndex);
  //   prepareBody(pathIndex, response);
  // } else if (access(pathIndex.c_str(), R_OK) < 0 &&
  //            (*dts.path)[dts.path->size() - 1] == '/' && autoindex == "on") {
  //   *dts.statusCode = E_200_OK;
  //   prepareFileList(*dts.path, dts, response);
  //   // return 0;
  // } else {
  //   throw(*dts.statusCode = E_404_NOT_FOUND);
  // }
  if (response.getBody().empty()) {
    *dts.statusCode = E_204_NO_CONTENT;
  }
}

bool GET::checkFile(std::string& path) {
  // 처음에 요청한 파일이 존재하며 파일인지 확인
  struct stat fileStat;

  if (stat(path.c_str(), &fileStat) == 0 && !(S_ISDIR(fileStat.st_mode)))
    return true;
  return false;
}

bool GET::checkDirectory(std::string& path) {
  // 처음에 요청한 Path가 디렉토리이며 접근 가능한 경우
  struct stat fileStat;

  if (stat(path.c_str(), &fileStat) == 0 && (S_ISDIR(fileStat.st_mode)))
    return true;
  return false;
}

bool GET::checkIndexFile(std::string& pathIndex) {
  struct stat fileStat;

  // 요청한 파일은 없고, index 파일이 존재하는지 확인
  if (stat(pathIndex.c_str(), &fileStat) == 0 && (S_ISREG(fileStat.st_mode)))
    return true;
  return false;
}

bool GET::checkAutoIndex(std::string& pathIndex, const std::string& autoindex) {
  struct stat fileStat;

  // 요청한 파일도 없고, index 파일도 없고, autoindex가 켜져있는지 확인
  if (stat(pathIndex.c_str(), &fileStat) < 0 && !(S_ISREG(fileStat.st_mode)) &&
      autoindex == "on")
    return true;
  return false;
}

std::vector<std::string> GET::getFileList(const std::string& path,
                                          RequestDts& dts) {
  DIR* dir;
  struct dirent* ent;
  std::vector<std::string> files;

  if ((*dts.originalPath)[(*dts.originalPath).size() - 1] != '/')
    *dts.originalPath += '/';
  if ((dir = opendir(path.c_str())) != NULL) {
    while ((ent = readdir(dir)) != NULL) {
      if (ent->d_name[0] == '.') continue;
      if (ent->d_type == DT_DIR)
        files.push_back((*dts.originalPath + ent->d_name + '/'));
      else
        files.push_back((*dts.originalPath + ent->d_name));
    }
    closedir(dir);
  } else {
    throw(*dts.statusCode = E_404_NOT_FOUND);
  }
  return files;
}

std::string GET::generateHTML(const std::vector<std::string>& files) {
  std::string html = "<html><body>";
  for (std::vector<std::string>::const_iterator it = files.begin();
       it != files.end(); ++it) {
    html += "<a href=\"" + *it + "\">" + *it + "</a><br>";
  }
  html += "</body></html>";
  return html;
}

void GET::fileHandler(const std::string& path) {
  size_t fd = size_t(std::fopen(path.c_str(), "r"));
  (void)fd;
  // error handling code
}

void GET::prepareFileList(const std::string& path, RequestDts& dts,
                          IResponse& response) {
  try {
    std::vector<std::string> files = getFileList(path, dts);
    response.setBody(generateHTML(files));
    response.setHeaderField("Content-Type", "text/html");
  } catch (int statusCode) {
    response.setBody("Could not open directory");
    throw(statusCode);
  }
}

void GET::prepareBody(const std::string& path, IResponse& response) {
  getContentType(path, response);
  const std::string& value = response.getFieldValue("Content-Type");
  if (value == "text/html" || value == "text/css" ||
      value == "application/json") {
    prepareTextBody(path, response);
  } else {
    prepareBinaryBody(path, response);
  }
}

void GET::prepareTextBody(const std::string& path, IResponse& response) {
  std::ifstream file(path.c_str(), std::ios::in);
  std::string buff;
  while (std::getline(file, buff)) {
    response.addBody(buff);
    response.addBody("\r\n");
  }
  file.close();
}

void GET::prepareBinaryBody(const std::string& path, IResponse& response) {
  std::ifstream file(path.c_str(), std::ios::binary);
  std::stringstream buffer;
  buffer << file.rdbuf();
  response.addBody(buffer.str());
  file.close();
}

void GET::createSuccessResponse(IResponse& response) {
  validateContentType(response);
  response.setHeaderField("Content-Length", itos(response.getBody().size()));
  response.assembleResponse();
#ifdef DEBUG_MSG_BODY
  std::cout << response.getResponse() << "\n";
#endif
  response.setResponseParsed();
}

void GET::validateContentType(IResponse& response) {
  const std::string& value = response.getFieldValue("Content-Type");
  if (value == "text/html" || value == "text/css" || value == "text/xml" ||
      value == "text/mathml" || value == "text/plain" ||
      value == "text/vnd.sun.j2me.app-descriptor" ||
      value == "text/vnd.wap.wml" || value == "text/x-component" ||
      value == "application/javascript" || value == "application/atom+xml" ||
      value == "application/rss+xml" || value == "application/json" ||
      value == "application/x-perl" || value == "application/json" ||
      value == "application/xhtml+xml" || value == "application/x-tcl" ||
      value == "application/xspf+xml") {
    response.setHeaderField("Content-Type", value + "; charset=UTF-8");
  }
}

void GET::getContentType(const std::string& path, IResponse& response) {
  std::string extension = path.substr(path.find_last_of(".") + 1);
  MimeTypesConfig& config = dynamic_cast<MimeTypesConfig&>(
      Config::getInstance().getMimeTypesConfig());
  try {
    std::cout << "extension::" << extension << std::endl;
    response.setHeaderField("Content-Type", config.getVariable(extension));
    return;
  } catch (ExceptionThrower::InvalidConfigException& e) {
    response.setHeaderField("Content-Type", "application/octet-stream");
    // response.setHeaderField("Content-Type", "text/plain");
  }
}

/**
 * @brief 특정 URL의 엔드포인트를 GET하는 함수
 *
 * @details
 *
 *
 * @param dts
 * @param response
 * @return true : true를 리턴하면 해당 트랜잭션을 더이상 진행하지 않음.
 * @return false : false를 리턴하면 해당 트랜잭션을 퍼블릭으로 넘어가서 진행.
 */
bool GET::getSpecificEndpoint(RequestDts& dts, IResponse& response) {
  if (*dts.is_session == false) return false;
  const std::string& originalPath = *dts.originalPath;
  try {
    SessionData& sessionData =
        Session::getInstance().getSessionData((*dts.cookieMap)["session_id"]);

    if (originalPath == "/enter.html") {
      getHome(dts);
    } else if (originalPath == "/asset/marin03.jpg") {
      getJangChoOrMarin(dts, sessionData);
    } else if (originalPath == "/gaepo.html") {
      getGaepo(dts, sessionData);
    } else if (originalPath == "/session") {
      getSessionData(response, sessionData);
      return true;
    }
  } catch (ExceptionThrower::SessionDataNotFound& e) {
#ifdef DEBUG_MSG
    std::cout << "session not found " << e.what() << std::endl;
#endif
    if (originalPath == "/") {
      getEnterPage(dts);
    } else if (originalPath == "/asset/marin03.jpg") {
      getJangCho(dts);
    }
  }
  return false;
}

void GET::getSessionData(IResponse& response, SessionData& sessionData) {
  response.setHeaderField("Content-Type", "application/json");

  std::string body =
      "[{\"entername\":\"" + sessionData.getData("entername") + "\"},";
  response.setBody(body);
  response.addBody(sessionData.getData("data"));
  response.addBody(",{\"fifteen\":\"" + sessionData.getData("fifteen") + "\"}");
  response.addBody("]");
}

void GET::getEnterPage(RequestDts& dts) {
  *dts.path = "/enter.html";
  throw(*dts.statusCode = E_302_FOUND);
}

void GET::getJangCho(RequestDts& dts) {
  *dts.path = "/asset/jangho.jpg";
  throw(*dts.statusCode = E_302_FOUND);
}

void GET::getJangChoOrMarin(RequestDts& dts, SessionData& sessionData) {
  if (sessionData.getData("fifteen") == "on") {
    return;
  } else {
    *dts.path = "/asset/jangho.jpg";
    throw(*dts.statusCode = E_302_FOUND);
  }
}

void GET::getHome(RequestDts& dts) {
  *dts.path = "/";
  throw(*dts.statusCode = E_302_FOUND);
}

void GET::getGaepo(RequestDts& dts, SessionData& sessionData) {
  if (sessionData.getData("fifteen") == "on") {
    return;
  } else {
    throw(*dts.statusCode = E_401_UNAUTHORIZED);
  }
}