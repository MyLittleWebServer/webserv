
#include "GET.hpp"

#include <dirent.h>
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

  if (checkFile(path)) {
    *dts.statusCode = E_200_OK;
    prepareBody(path, response);
  } else if (checkIndexFile(pathIndex)) {
    *dts.statusCode = E_200_OK;
    prepareBody(pathIndex, response);
  } else if (checkAutoIndex(path, pathIndex, autoindex)) {
    *dts.statusCode = E_200_OK;
    prepareFileList(path, dts, response);
  } else {
    throw(*dts.statusCode = E_404_NOT_FOUND);
  }
}

bool GET::checkFile(std::string& path) {
  // 처음에 요청한 파일이 존재하는지 확인
  if (access(path.c_str(), R_OK) == 0 && path[path.size() - 1] != '/')
    return true;
  return false;
}

bool GET::checkIndexFile(std::string& pathIndex) {
  // 요청한 파일은 없고, index 파일이 존재하는지 확인
  if (access(pathIndex.c_str(), R_OK) == 0 &&
      pathIndex[pathIndex.size() - 1] != '/')
    return true;
  return false;
}

bool GET::checkAutoIndex(std::string& path, std::string& pathIndex,
                         const std::string& autoindex) {
  // 요청한 파일도 없고, index 파일도 없고, autoindex가 켜져있는지 확인
  if (access(pathIndex.c_str(), R_OK) < 0 && path[path.size() - 1] == '/' &&
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
  Session& session = Session::getInstance();

  try {
    if (*dts.originalPath == "/") {
      getHome(dts);
    } else if (*dts.originalPath == "/enter.html") {
      getEnterPage(dts);
    } else if (*dts.originalPath == "/asset/marin03.jpg") {
      getMarin(dts, session);
    } else if (*dts.originalPath == "/gaepo.html") {
      getGaepo(dts, session);
    }

    SessionData& sessionData =
        session.getSessionData((*dts.cookieMap)["session_id"]);
    if (*dts.originalPath == "/session") {
      getSessionData(response, sessionData);
      return true;
    }
  } catch (ExceptionThrower::SessionDataNotFound& e) {
#ifdef DEBUG_MSG
    std::cout << "session not found " << e.what() << std::endl;
#endif
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

void GET::getHome(RequestDts& dts) {
  if ((*dts.cookieMap).find("session_id") == (*dts.cookieMap).end()) {
    *dts.path = "/enter.html";
    throw(*dts.statusCode = E_302_FOUND);
  }
}

void GET::getMarin(RequestDts& dts, Session& session) {
  if ((*dts.cookieMap).find("session_id") == (*dts.cookieMap).end()) {
    *dts.path = "/asset/jangho.jpg";
    throw(*dts.statusCode = E_302_FOUND);
  }
  SessionData& sessionData =
      session.getSessionData((*dts.cookieMap)["session_id"]);
  if (sessionData.getData("fifteen") == "") {
    *dts.path = "/asset/jangho.jpg";
    throw(*dts.statusCode = E_302_FOUND);
  }
}

void GET::getGaepo(RequestDts& dts, Session& session) {
  SessionData& sessionData =
      session.getSessionData((*dts.cookieMap)["session_id"]);
  if (sessionData.getData("fifteen") != "on") {
    throw(*dts.statusCode = E_401_UNAUTHORIZED);
  }
}

void GET::getEnterPage(RequestDts& dts) {
  if ((*dts.cookieMap).find("session_id") != (*dts.cookieMap).end()) {
    *dts.path = "/";
    throw(*dts.statusCode = E_302_FOUND);
  }
}