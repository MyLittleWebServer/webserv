/**
 * @file Status.cpp
 * @brief HTTP 응답 코드와 메세지를 정의한 소스파일입니다.
 * @details
 * @author chanhihi
 * @date 2023-07-20
 * @copyright Copyright (c) 2023
 */

#include "Status.hpp"

#include <string>

/**
 * @brief HTTP 응답 코드와 메세지를 저장하는 구조체입니다.
 *
 * @details
 * 1. message : 응답 메세지
 * 2. code : 응답 코드
 * 3. contentLength : 응답 바디의 길이
 * 4. body : 응답 바디
 *
 * @see https://developer.mozilla.org/ko/docs/Web/HTTP/Status
 *
 * @author chanhihi
 * @date 2021-07-20
 */
msgArray statusInfo[] = {
    msgArray("OK", "200", 0, ""),
    msgArray("Created", "201", 0, ""),
    msgArray("Accepted", "202", 16, "Your request is being processed."),
    msgArray("No Content", "204", 0, ""),
    msgArray("Moved Permanently", "301", 0, ""),
    msgArray("Found", "302", 0, ""),
    msgArray("See Other", "303", 0, ""),
    msgArray("Not Modified", "304", 0, ""),
    msgArray("Temporary Redirect", "307", 0, ""),
    msgArray("Permanent Redirect", "308", 0, ""),
    msgArray("Bad Request", "400", 11, "Invalid request."),
    msgArray("Unauthorized", "401", 12, "Authentication Required."),
    msgArray("Forbidden", "403", 9, "Access forbidden."),
    msgArray("Not Found", "404", 9, "Not Found"),
    msgArray("Method Not Allowed", "405", 19, "Method Not Allowed."),
    msgArray("Request Timeout", "408", 18, "Request Timeout."),
    msgArray("Conflict", "409", 18, "Resource conflict."),
    msgArray("Request Entity Too Large", "413", 27,
             "Request Entity Too Large."),
    msgArray("URI Too Long", "414", 0, "NULL"),
    msgArray("Unsupported Media Type", "415", 24, "Unsupported Media Type."),
    msgArray("Internal Server Error", "500", 21, "Internal Server Error"),
    msgArray("Not_Implemented", "501", 19, "Not Implemented Yet."),
    msgArray("Bad Gateway", "502", 11, "Bad Gateway"),
    msgArray("Service Unavailable", "503", 21,
             "Service Temporarily Unavailable"),
    msgArray("Gateway Timeout", "504", 20, "Gateway Timeout Error"),
    msgArray("HTTP Version Not Supported", "505", 27,
             "HTTP Version Not Supported")};
