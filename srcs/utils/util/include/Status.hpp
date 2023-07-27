/**
 * @file Status.hpp
 * @author chanhihi
 * @brief HTTP 응답 코드와 메세지를 정의한 헤더파일입니다.
 * @version 0.1
 * @date 2023-07-20
 *
 * @copyright Copyright (c) 2023
 */
#pragma once

/**
 * @brief HTTP 응답 코드와 메세지를 저장하는 구조체입니다.
 *
 * @details
 * 1. message : 응답 메세지
 * 2. code : 응답 코드
 * 3. contentLength : 응답 바디의 길이
 * 4. body : 응답 바디
 * 5. dummy : 구조체의 크기를 맞추기 위한 dummy
 */
struct msgArray {
  const char *message;
  const char *code;
  int contentLength;
  const char *body;
  char dummy[4];
  msgArray(const char *msg, const char *c, int l, const char *body)
      : message(msg), code(c), contentLength(l), body(body) {}
};

/**
 * @brief HTTP 응답 코드를 정의한 enum입니다.
 *
 * @details
 * 1xx : Informational
 * 2xx : Success
 * 3xx : Redirection
 * 4xx : Client Error
 * 5xx : Server Error
 *
 * @see https://developer.mozilla.org/ko/docs/Web/HTTP/Status
 *
 * @author chanhihi
 * @date 2021-07-20
 */
enum Status {
  E_200_OK,
  E_201_CREATED,
  E_202_ACCEPTED,
  E_204_NO_CONTENT,
  E_301_MOVED_PERMANENTLY,
  E_302_FOUND,
  E_303_SEE_OTHER,
  E_304_NOT_MODIFIED,
  E_307_TEMPORARY_REDIRECT,
  E_308_PERMANENT_REDIRECT,
  E_400_BAD_REQUEST,
  E_401_UNAUTHORIZED,
  E_403_FORBIDDEN,
  E_404_NOT_FOUND,
  E_405_METHOD_NOT_ALLOWED,
  E_408_REQUEST_TIMEOUT,
  E_409_CONFLICT,
  E_413_REQUEST_ENTITY_TOO_LARGE,
  E_414_URI_TOO_LONG,
  E_415_UNSUPPORTED_MEDIA_TYPE,
  E_418_IM_A_TEAPOT,
  E_500_INTERNAL_SERVER_ERROR,
  E_501_NOT_IMPLEMENTED,
  E_502_BAD_GATEWAY,
  E_503_SERVICE_UNAVAILABLE,
  E_504_GATEWAY_TIMEOUT,
  E_505_HTTP_VERSION_NOT_SUPPORTED
};

extern msgArray statusInfo[];
