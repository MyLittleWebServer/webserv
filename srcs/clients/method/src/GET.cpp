
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

/**
 * @brief GET 메소드를 처리하는 함수
 *
 * @details
 * GET 요청은 PATH를 통해 파일을 찾고, 파일또는 디렉토리가
 * 존재하면 해당 파일또는 디렉토리를 읽어서 응답합니다. 파일이 존재하지 않으면
 * 404를 응답합니다.
 *
 * @param dts
 * @param response
 */
void GET::doRequest(RequestDts& dts, IResponse& response) {
  handlePath(dts, response);
  bodyCheck(dts, response);
}

/**
 * @brief GET 요청으로 들어온 PATH를 관리하는 함수
 *
 * @details
 * 미리 지정해둔 특정 URL의 엔드포인트가 request되면 해당 엔드포인트의 로직에
 * 따라 처리합니다. 그 외의 경우에는 getPublicEndpoint()를 호출하여 퍼블릭
 * 엔드포인트로써 로직을 처리합니다.
 *
 * @param dts
 * @param response
 */
void GET::handlePath(RequestDts& dts, IResponse& response) {
  if (getSpecificEndpoint(dts, response))
    return;
  else
    getPublicEndpoint(dts, response);
}

/**
 * @brief 지정외 모든 URL의 엔드포인트를 GET하는 함수
 *
 * @details
 * 지정외 모든 URL의 엔드포인트는 퍼블릭 엔드포인트로써 처리합니다. 들어온 URL은
 * 다음과 같은 로직에따라 처리합니다.
 * 1. 파일인 경우
 *  - 파일을 읽어서 응답합니다.
 *  - 파일이 존재하지 않으면 404를 응답합니다.
 * 2. 디렉토리인 경우
 *  - index 파일이 존재하면 index 파일을 읽어서 응답합니다.
 *  - index 파일이 존재하지 않고, autoindex가 켜져있으면 디렉토리의 파일 목록을
 * 읽어서 응답합니다.
 *  - index 파일이 존재하지 않고, autoindex가 꺼져있으면 404를 응답합니다.
 *
 * @param dts
 * @param response
 */
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
  } else if (checkDirectory(path)) {
    if (checkIndexFile(pathIndex)) {
      *dts.statusCode = E_200_OK;
      prepareBody(pathIndex, response);
    } else if (checkAutoIndex(pathIndex, autoindex)) {
      *dts.statusCode = E_200_OK;
      prepareFileList(path, dts, response);
    } else {
      throw(*dts.statusCode = E_404_NOT_FOUND);
    }
  } else {
    throw(*dts.statusCode = E_404_NOT_FOUND);
  }
}

/**
 * @brief 요청한 PATH가 파일인지 확인하는 함수
 *
 * @see stat 함수를 사용하여 파일인지 확인
 * @see S_ISREG regular file인지 확인하는 매크로
 *
 * @param path
 * @return true
 * @return false
 */
bool GET::checkFile(std::string& path) {
  struct stat fileStat;

  if (stat(path.c_str(), &fileStat) == 0 && (S_ISREG(fileStat.st_mode)))
    return true;
  return false;
}

/**
 * @brief 요청한 PATH가 디렉토리인지 확인하는 함수
 *
 * @see stat 함수를 사용하여 디렉토리인지 확인
 * @see S_ISDIR directory인지 확인하는 매크로
 *
 * @param path
 * @return true
 * @return false
 */
bool GET::checkDirectory(std::string& path) {
  struct stat fileStat;

  if (stat(path.c_str(), &fileStat) == 0 && (S_ISDIR(fileStat.st_mode)))
    return true;
  return false;
}

/**
 * @brief 요청한 PATH에 index 파일이 존재하는지 확인하는 함수
 *
 * @see stat 함수를 사용하여 파일인지 확인
 * @see S_ISREG regular file인지 확인하는 매크로
 *
 * @param pathIndex : index 파일의 경로
 * @return true
 * @return false
 */
bool GET::checkIndexFile(std::string& pathIndex) {
  struct stat fileStat;
  if (stat(pathIndex.c_str(), &fileStat) == 0 && (S_ISREG(fileStat.st_mode)))
    return true;
  return false;
}

/**
 * @brief 요청한 PATH에 index 파일이 존재하지 않고, autoindex가 켜져있는지
 * 확인하는 함수
 *
 * @see stat 함수를 사용하여 파일인지 확인
 * @see S_ISREG regular file인지 확인하는 매크로
 *
 * @param pathIndex : index 파일의 경로
 * @param autoindex : autoindex가 켜져있는지 확인하는 변수
 * @return true
 * @return false
 */
bool GET::checkAutoIndex(std::string& pathIndex, const std::string& autoindex) {
  struct stat fileStat;

  if (stat(pathIndex.c_str(), &fileStat) < 0 && autoindex == "on") return true;
  return false;
}

/**
 * @brief AutoIndex 가 On인 경우 디렉토리의 파일 목록을 읽어오는 함수
 *
 * @details
 * 디렉토리의 파일 목록을 읽어서 HTML 형식으로 만들어서 리턴합니다.
 *
 * @param path
 * @param dts
 * @return std::vector<std::string>
 */
std::vector<std::string> GET::getFileList(const std::string& path,
                                          RequestDts& dts) {
  DIR* dir;
  struct dirent* ent;
  std::vector<std::string> files;

  if ((*dts.originalPath)[(*dts.originalPath).size() - 1] != '/')
    *dts.originalPath += '/';
  if ((dir = opendir(path.c_str())) != NULL) {
    while ((ent = readdir(dir)) != NULL) {
      if (ent->d_name[0] == '.' && ent->d_namlen == 1) continue;
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

/**
 * @brief AutoIndex 가 On인 경우 디렉토리의 파일 목록을 HTML 형식으로 만드는
 * 함수
 *
 * @param path
 * @param files
 * @return std::string
 */
std::string GET::generateHTML(const std::string& path,
                              const std::vector<std::string>& files) {
  std::string html = HTML_HEAD_TITLE(path) + STYLE + HTML_BODY(path);
  for (std::vector<std::string>::const_iterator it = files.begin();
       it != files.end(); ++it) {
    html += "<a href=\"" + *it + "\" style =\"font-size:1.5rem;\">" +
            ((*it)[it->size() - 1] == '/' ? "📁 " : "📄 ") + *it + "</a>";
  }
  html += HTML_BODY_END;
  return html;
}

/**
 * @brief AutoIndex 가 On인 경우 디렉토리의 파일 목록을 response에 담는 함수
 *
 * @param path
 * @param dts
 * @param response
 */
void GET::prepareFileList(const std::string& path, RequestDts& dts,
                          IResponse& response) {
  try {
    std::vector<std::string> files = getFileList(path, dts);
    response.setBody(generateHTML(path, files));
    response.setHeaderField("Content-Type", "text/html");
  } catch (int statusCode) {
    response.setBody("Could not open directory");
    throw(statusCode);
  }
}

/**
 * @brief 파일을 읽어서 response에 담는 함수
 *
 * @details text, binary 파일을 구분하여 읽어서 response에 담습니다.
 *
 * @param path
 * @param response
 */
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

/**
 * @brief text 파일을 읽어서 response에 담는 함수
 *
 * @details
 * 파일을 한줄씩 읽어서 response에 담습니다.
 *
 * @see std::ifstream 클래스를 사용하여 파일을 읽습니다.
 *
 * @param path
 * @param response
 */
void GET::prepareTextBody(const std::string& path, IResponse& response) {
  std::ifstream file(path.c_str(), std::ios::in);
  std::string buff;
  while (std::getline(file, buff)) {
    response.addBody(buff);
    response.addBody("\r\n");
  }
  file.close();
}

/**
 * @brief binary 파일을 읽어서 response에 담는 함수
 *
 * @see std::ifstream 클래스를 사용하여 파일을 binary로 읽습니다.
 *
 * @param path
 * @param response
 */
void GET::prepareBinaryBody(const std::string& path, IResponse& response) {
  std::ifstream file(path.c_str(), std::ios::binary);
  std::stringstream buffer;
  buffer << file.rdbuf();
  response.addBody(buffer.str());
  file.close();
}

/**
 * @brief response의 body가 비어있는지 확인하는 함수
 *
 * @param dts
 * @param response
 */
void GET::bodyCheck(RequestDts& dts, IResponse& response) {
  if (response.getBody().empty()) {
    *dts.statusCode = E_204_NO_CONTENT;
  }
}

/**
 * @brief 성공적인 응답을 만드는 함수
 *
 * @details
 * content type을 결정 하여 Content-Type에 추가합니다.
 * content length를 결정하여 Content-Length에 추가합니다.
 * response를 조립합니다.
 * response를 파싱합니다.
 *
 * @param response
 */
void GET::createSuccessResponse(IResponse& response) {
  validateContentType(response);
  response.setHeaderField("Content-Length", itos(response.getBody().size()));
  response.assembleResponse();
  response.setResponseParsed();
}

/**
 * @brief Content-Type을 확인하여 charset을 추가하는 함수
 *
 * @details
 * content type이 UTF-8 인코딩을 따르는 경우 charset을 추가합니다.
 *
 * @param response
 */
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

/**
 * @brief 파일의 확장자를 통해 Content-Type을 확인하는 함수
 *
 * @details
 * 파일의 확장자를 통해 Content-Type을 확인합니다. 확장자가 없는 경우
 * octet-stream을 response에 담습니다.
 *
 * @param path
 * @param response
 */
void GET::getContentType(const std::string& path, IResponse& response) {
  std::string extension = path.substr(path.find_last_of(".") + 1);
  MimeTypesConfig& config = dynamic_cast<MimeTypesConfig&>(
      Config::getInstance().getMimeTypesConfig());
  try {
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
 * 특정 URL의 엔드포인트는 세션을 통해 입장 여부를 확인합니다. 세션이 존재하지
 * 않으면 401을 응답합니다. 세션이 존재하면 해당 엔드포인트의 로직에 따라
 * 처리합니다.
 *
 * 1. /enter.html
 * 2. /asset/marin03.jpg
 * 3. /gaepo.html
 * 4. /session
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
    if (originalPath == "/") {
      getEnterPage(dts);
    } else if (originalPath == "/asset/marin03.jpg") {
      getJangCho(dts);
    }
  }
  return false;
}

/**
 * @brief 세션 데이터를 응답하는 함수
 *
 * @details
 * enter name, fifteen, data를 응답합니다.
 *
 * @param response
 * @param sessionData
 */
void GET::getSessionData(IResponse& response, SessionData& sessionData) {
  response.setHeaderField("Content-Type", "application/json");

  std::string body =
      "[{\"entername\":\"" + sessionData.getData("entername") + "\"},";
  response.setBody(body);
  response.addBody(sessionData.getData("data"));
  response.addBody(",{\"fifteen\":\"" + sessionData.getData("fifteen") + "\"}");
  response.addBody("]");
}

/**
 * @brief enter.html을 응답하는 함수
 *
 * @param dts
 */
void GET::getEnterPage(RequestDts& dts) {
  *dts.path = "/enter.html";
  throw(*dts.statusCode = E_302_FOUND);
}

/**
 * @brief jangho.jpg를 응답하는 함수
 *
 * @param dts
 */
void GET::getJangCho(RequestDts& dts) {
  *dts.path = "/asset/jangho.jpg";
  throw(*dts.statusCode = E_302_FOUND);
}

/**
 * @brief jangho.jpg 또는 marin03.jpg를 응답하는 함수
 *
 * @param dts
 * @param sessionData
 */
void GET::getJangChoOrMarin(RequestDts& dts, SessionData& sessionData) {
  if (sessionData.getData("fifteen") == "on") {
    return;
  } else {
    *dts.path = "/asset/jangho.jpg";
    throw(*dts.statusCode = E_302_FOUND);
  }
}

/**
 * @brief /를 응답하는 함수
 *
 * @param dts
 */
void GET::getHome(RequestDts& dts) {
  *dts.path = "/";
  throw(*dts.statusCode = E_302_FOUND);
}

/**
 * @brief gaepo.html를 응답하는 함수
 *
 * @param dts
 * @param sessionData
 */
void GET::getGaepo(RequestDts& dts, SessionData& sessionData) {
  if (sessionData.getData("fifteen") == "on") {
    return;
  } else {
    throw(*dts.statusCode = E_401_UNAUTHORIZED);
  }
}
