#include "PUT.hpp"

#include <fstream>

#include "Utils.hpp"

PUT::PUT(void) {}

PUT::~PUT(void) {}

void PUT::doRequest(RequestDts& dts, IResponse& response) {
  std::cout << " >>>>>>>>>>>>>>> PUT\n";
  std::cout << "path: " << *dts.path << "\n";
  std::cout << "content-type: " << (*dts.headerFields)["content-type"] << "\n";
  std::cout << "content-length: " << (*dts.headerFields)["content-length"]
            << "\n";
  /*
      if path does not include '/':
          putPath = dts.path + "/" # regard this path as real path

      if (checkUniqueIdentifier(dts, putPath) == true):
          changeFile()
          statusCode = 200 OK, 204 No Content
      elif:
          generateFile()
          statusCode = 201 created
  */
  (void)response;
  // (void)dts;
}

void PUT::checkUniqueIdentifier(RequestDts& dts) {
  // (void)dts;
  std::string tmpPath = *dts.path;
  std::cout << "tmpPath : " << tmpPath << "\n";
  size_t slashPos = tmpPath.find('/');
  if (slashPos == std::string::npos) throw(*dts.statusCode = E_400_BAD_REQUEST);
  std::cout << "tmpSlashPos : " << tmpPath[slashPos + 1] << "\n";
  // if (tmpPath[slashPos + 1] == '\0')
  // _title = file
  // get mimetype
  // _content = (*dts.body)
  // createTextFile()
  _title = tmpPath.substr(slashPos + 1);
  // if (_title in _uniqueID)
  // return true
  // else
  // _uniqueID.push_back(_title);
  std::cout << "uniqueID" << _uniqueID << "\n";
}

void PUT::createSuccessResponse(IResponse& response) {
  response.assembleResponse();
  response.setResponseParsed();
}
/*
POST : create only
PUT : update if not existed then crated

1. 클라이언트는 PUT 요청을 보낼 때 리소스의 고유 식별자를 요청 URL 또는 경로에
포함시킵니다. 이 식별자는 해당 리소스를 유일하게 식별할 수 있는 정보로
사용됩니다.

2. 서버는 받은 PUT 요청의 URL 또는 경로에서 리소스의 고유 식별자를 추출합니다.

3. 서버는 추출한 고유 식별자를 사용하여 해당 리소스가 이미 존재하는지 여부를
확인합니다.

4-1. 리소스가 존재하는 경우: 서버는 해당 리소스를 업데이트하고 PUT 요청의
데이터를 기존 리소스에 적용합니다.

4-2. 리소스가 존재하지 않는 경우: 서버는 새로운 리소스를 생성하고 고유 식별자를
가진 새로운 리소스를 생성합니다. 만약 리소스를 생성하기 위해 요청 URL에 새로운
식별자를 포함시켰다면, 해당 식별자를 새 리소스의 고유 식별자로 사용합니다.

5. 적절한 응답: 서버는 적절한 상태 코드와 응답 본문을 클라이언트로 반환하여
작업의 성공 여부를 알려줍니다.
*/
