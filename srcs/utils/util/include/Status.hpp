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
  OK,
  CREATED,
  ACCEPTED,
  NO_CONTENT,
  MOVED_PERMANENTLY,
  FOUND,
  SEE_OTHER,
  NOT_MODIFIED,
  TEMPORARY_REDIRECT,
  PERMANENT_REDIRECT,
  BAD_REQUEST,
  UNAUTHORIZED,
  FORBIDDEN,
  NOT_FOUND,
  METHOD_NOT_ALLOWED,
  REQUEST_TIMEOUT,
  CONFLICT,
  REQUEST_ENTITY_TOO_LARGE,
  INTERNAL_SERVER_ERROR,
  NOT_IMPLEMENTED,
  BAD_GATEWAY,
  SERVICE_UNAVAILABLE,
  GATEWAY_TIMEOUT,
  HTTP_VERSION_NOT_SUPPORTED
};

extern msgArray statusInfo[];