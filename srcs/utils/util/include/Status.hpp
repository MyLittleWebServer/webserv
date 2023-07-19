#pragma once

struct msgArray {
  const char *message;
  const char *code;
  int contentLength;
  const char *body;
  char dummy[4];
  msgArray(const char *msg, const char *c, int l, const char *body)
      : message(msg), code(c), contentLength(l), body(body) {}
};

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
  E_500_INTERNAL_SERVER_ERROR,
  E_501_NOT_IMPLEMENTED,
  E_502_BAD_GATEWAY,
  E_503_SERVICE_UNAVAILABLE,
  E_504_GATEWAY_TIMEOUT,
  E_505_HTTP_VERSION_NOT_SUPPORTED
};

extern msgArray statusInfo[];
