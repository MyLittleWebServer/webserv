#include "AMethod.hpp"

/* TODO
1. 헤더 / 바디 파싱
2. 헤더 : 요청라인 3부분 으로 나뉨
	- Method Path Version_of_protocol -> Method 만 불러서 리턴해주기

3. 바디 : ':' 기준으로 앞, 뒤 나눠서 key,value 값으로 매핑해주기
	- 가능하다면 키값을 enum 타입으로 지정해서 찾기 편리하게 해주는 역할
*/


/* GET request example
GET /path/to/resource HTTP/1.1
Host: example.com
Content-Type: application/x-www-form-urlencoded

key1=value1&key2=value2&key3=value3
*/

AMethod::AMethod(std::string &request) : _request(request), _responseFlag(false) {}

AMethod::~AMethod() {}

// void AMethod::parseRequest(std::string _request, std::string _method, \
// std::map<std::string, std::string> _headMapper)
// {
// 	size_t headerEnd = _request.find("\r\n\r\n");
// 	std::string header = _request.substr(0, headerEnd);
// 	std::string body = _request.substr(headerEnd + 4);

// 	size_t requestFindMethod = _request.find("\r\n");
// 	size_t requestFindFirstBlank = _request.find(' ');
// 	std::string firstLinefeed = _request.substr(0, requestFindMethod);
	
// 	this->_method = firstLinefeed.substr(0, requestFindFirstBlank); // method 할당
// }

void AMethod::requestVector(std::string _request, std::vector<std::string> _requestVector)
{
	size_t startPos = 0;
	size_t delimeter = _request.find("\r\n");
	while (delimeter != std::string::npos)
	{
		std::string chunk = _request.substr(startPos, delimeter);
		_requestVector.push_back(chunk);
		startPos = delimeter + 1;
		size_t delimeter = _request.find("\r\n", startPos);
	}
}

void AMethod::requestParser(std::vector<std::string> _requestVector)
{
	std::string firstLine = _requestVector[0];
	std::istringstream iss(firstLine);

	iss >> this->_method >> this->_path >> this->_protocol;
}

void AMethod::requestSplitByColon(std::vector<std::string> _requestVector)
{
}

const std::string &AMethod::getResponse(void) const
{
	return (this->_response);
}

bool AMethod::getResponseFlag(void) const
{
	return (this->_responseFlag);
}