#include "Status.hpp"

#include <string>

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
