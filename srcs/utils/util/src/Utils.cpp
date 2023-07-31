/**
 * @file Utils.cpp
 * @author chanhihi
 * @brief 유틸리티 함수를 정의한 소스파일입니다.
 * @version 0.1
 * @date 2023-07-20
 *
 * @copyright Copyright (c) 2023
 */

#include "Utils.hpp"

#include <ctime>

/**
 * @brief 에러 메세지를 출력하고 프로그램을 종료합니다.
 *
 * @details
 * 1. 에러 메세지를 출력합니다.
 * 2. main의 catch(...)블록으로 이동합니다.
 *
 * @param message
 *
 * @author chanhihi
 * @date 2023-07-21
 */
void throwWithErrorMessage(const std::string &message) {
  std::cerr << message << " : " << strerror(errno) << std::endl;
  throw(errno);
}

/**
 * @brief 현재 이벤트의 포트를 반환합니다.
 *
 * @details
 * addr 구조체를 생성하여 getsockname()을 호출합니다.
 * getsockname()은 현재 FD를 토대로 소켓의 sockaddr_in를 가져옵니다.
 * 그리고 sockaddr_in 구조체를 통해 포트를 가져옵니다.
 * 가져온 sin_port는 네트워크 바이트 순서로 되어있기 때문에 ntohs()를 통해
 * 호스트 바이트 순서로 변환하여 반환합니다.
 *
 * @exception std::runtime_error getsockname()이 실패하면 예외를 던집니다.
 *
 * @param ident
 * @return short
 */
short getBoundPort(const uintptr_t ident) {
  struct sockaddr_in addr;
  socklen_t addr_len = sizeof(addr);
  if (getsockname(ident, (struct sockaddr *)&addr, &addr_len) == -1)
    throw std::runtime_error("getsockname() error\n");
#ifdef DEBUG_MSG
  std::cout << "getsockname: " << ntohs(addr.sin_port) << std::endl;
#endif
  return (ntohs(addr.sin_port));
}

/**
 * @brief 현재시간을 GMT 포맷으로 반환합니다.
 *
 * @details
 * 1. 현재 시간을 가져옵니다.
 * 2. std::time_t를 gmtime을 통해 std::tm(GMT 포맷)으로 변환합니다.
 * 4. std::strftime()을 통해 std::tm을 문자열로 변환하여 buffer 저장.
 *
 * @see GMT (Greenwich Mean Time) 천문대를 기준으로 한 세계 표준시
 * @see UTC (Coordinated Universal Time, 협정 세계시)
 * @see RFC 2616 (HTTP/1.1 스펙)
 * @see RFC 1123
 *
 * @return std::string
 */
std::string getCurrentTime() {
  std::time_t t = std::time(NULL);
  std::tm *timePtr = std::gmtime(&t);

  char buffer[1000];
  std::strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S GMT", timePtr);

  return (std::string(buffer));
}

int getTimeOfDay() { return std::time(NULL); }

/**
 * @brief 문자열을 소문자로 변환합니다.
 *
 * @param str
 * @return std::string
 */
std::string toLowerString(std::string str) {
  std::string::iterator it = str.begin();
  std::string::iterator end;
  if (str.size() > 4) {
    end = str.end() - 4;

    while (it < end) {
      *it = std::tolower(*it);
      *(it + 1) = std::tolower(*(it + 1));
      *(it + 2) = std::tolower(*(it + 2));
      *(it + 3) = std::tolower(*(it + 3));
      it += 4;
    }
  }
  end = str.end();
  while (it != end) {
    *it = std::tolower(*it);
    ++it;
  }
  return str;
}

/**
 * @brief 문자열을 trimOWS합니다.
 *
 * @param str
 * @return std::string
 */
std::string ft_trimOWS(std::string &str) {
  std::string result = str;
  size_t pos = result.find_first_not_of(" \t");
  if (pos != std::string::npos) {
    result.erase(0, pos);
  }
  pos = result.find_last_not_of(" \t");
  if (pos != std::string::npos) {
    result.erase(pos + 1);
  }
  return result;
}

/**
 * @brief 문자열을 trim합니다.
 *
 * @param str
 * @return std::string
 */
std::string ft_trim(const std::string &str) {
  if (str.empty()) return str;
  std::string result = str;
  size_t pos = 0;
  while (pos < result.size() && std::isspace(result[pos])) pos++;
  result.erase(0, pos);
  pos = result.size() - 1;
  while (pos > 0 && std::isspace(result[pos])) pos--;
  result.erase(pos + 1);
  return result;
}

/**
 * @brief 문자열을 delim을 기준으로 split합니다.
 *
 * @param str
 * @param delim
 * @return std::vector<std::string>
 */
std::vector<std::string> ft_split(const std::string &str, char delim) {
  std::vector<std::string> result;
  std::istringstream iss(str);
  std::string token;

  while (std::getline(iss, token, delim)) {
    if (token.empty()) continue;
    result.push_back(token);
  }
  return result;
}

std::vector<std::string> ft_split(const std::string &s,
                                  const std::string &delimiter) {
  size_t pos_start = 0, pos_end, delim_len = delimiter.length();
  std::string token;
  std::vector<std::string> res;

  while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos) {
    token = s.substr(pos_start, pos_end - pos_start);
    pos_start = pos_end + delim_len;
    res.push_back(token);
  }

  res.push_back(s.substr(pos_start));
  return res;
}

/**
 * @brief 벡터에서 값으로 인덱스를 찾아 반환합니다.
 *
 * @param vec
 * @param str
 * @return size_t
 */
size_t find_index(std::vector<std::string> &vec, std::string &str) {
  size_t i = 0;
  while (i < vec.size()) {
    if (vec[i] == str) return i;
    i++;
  }
  return std::string::npos;
}

std::string generateRandomString() {
  static std::string charset =
      "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
  std::string result;
  std::srand(std::time(0));

  for (int i = 0; i < 32; ++i)
    result.push_back(charset[std::rand() % charset.size()]);
  return result;
}
