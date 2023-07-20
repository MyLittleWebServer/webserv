#include "OPTIONS.hpp"

OPTIONS::OPTIONS() {}

OPTIONS::~OPTIONS() {}

void OPTIONS::doRequest(RequestDts& dts, IResponse& response) {
  if (*dts.path == "*") {
    makeServerOptions(response);
  } else {
    makeLocationOption(dts, response);
  }
  response.setHeaderField("Server", (*dts.matchedServer)->getServerName());
}

void OPTIONS::makeServerOptions(IResponse& response) {
  response.setHeaderField("Allow", "GET, HEAD, POST, PUT, DELETE, OPTIONS");
}

void OPTIONS::makeLocationOption(RequestDts& dts, IResponse& response) {
  const std::map<std::string, bool>& methodInfo =
      (*dts.matchedLocation)->getAllowMethod();
  std::map<std::string, bool>::const_iterator start = methodInfo.begin();
  std::map<std::string, bool>::const_iterator end = methodInfo.end();
  std::string allowMethods;
  for (; start != end; ++start) {
    if (start->second) {
      allowMethods += start->first;
      allowMethods += ", ";
    }
  }
  if (allowMethods.size() > 2)
    allowMethods = allowMethods.substr(0, allowMethods.size() - 2);
  response.setHeaderField("Allow", allowMethods);
}

void OPTIONS::createSuccessResponse(IResponse& response) {
  response.setStatusCode(E_200_OK);
  response.setHeaderField("Content-Length", "0");
  response.setBody("");
  response.assembleResponse();
  response.setResponseParsed();
}
