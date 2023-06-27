#include "Status.hpp"

#include <string>

msgArray statusInfo[] = {
    msgArray("OK", "200", 0, NULL),
    msgArray("Created", "201", 0, NULL),
    msgArray("Accepted", "202", 16, "Your request is being processed."),
    msgArray("No Content", "204", 0, NULL),
    msgArray("Moved Permanently", "301", 0, NULL),
    msgArray("Found", "302", 0, NULL),
    msgArray("See Other", "303", 0, NULL),
    msgArray("Not Modified", "304", 0, NULL),
    msgArray("Temporary Redirect", "307", 0, NULL),
    msgArray("Permanent Redirect", "308", 0, NULL),
    msgArray("Bad Request", "400", 11, "Invalid request."),
    msgArray("Unauthorized", "401", 12, "Authentication Required."),
    msgArray("Forbidden", "403", 9, "Access forbidden."),
    msgArray("Not Found", "404", 9, "Not Found"),
    msgArray("Method Not Allowed", "405", 19, "Method Not Allowed."),
    msgArray("Request Timeout", "408", 18, "Request Timeout."),
    msgArray("Conflict", "409", 18, "Resource conflict."),
    msgArray("Request Entity Too Large", "413", 27,
             "Request Entity Too Large."),
    msgArray("Internal Server Error", "500", 21, "Internal Server Error"),
    msgArray("Not_Implemented", "501", 19, "Not Implemented Yet."),
    msgArray("Bad Gateway", "502", 11, "Bad Gateway"),
    msgArray("Service Unavailable", "503", 21,
             "Service Temporarily Unavailable"),
    msgArray("Gateway Timeout", "504", 20, "Gateway Timeout Error"),
    msgArray("HTTP Version Not Supported", "505", 27,
             "HTTP Version Not Supported")};
