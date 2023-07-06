#pragma once

/**
 * @brief CGI 인터페이스
 *
 * excute 이후 getCgiResult 호출로
 * 비동기로 처리된 CGI 프로그램 실행 결과를 얻어올 수 있다.
 *
 * @author middlefitting
 * @date 2023.07.05
 */
class ICGI {
 public:
  virtual ~ICGI(){};
};
