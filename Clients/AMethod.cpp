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

AMethod::AMethod() {}

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

void AMethod::splitLinesByCRLF(void)
{
	size_t pos = 0;
	size_t delimeter = this->_request.find("\r\n");
	while (delimeter != std::string::npos)
	{
		std::string chunk = this->_request.substr(pos, delimeter);
		this->_linesBuffer.push_back(chunk);
		pos = delimeter + 2;
		delimeter = this->_request.find("\r\n", pos);
	}
}

void AMethod::parseRequestLine(void)
{
	std::string firstLine(this->_linesBuffer[0]);
	this->_linesBuffer.pop_front();

	std::istringstream iss(firstLine);
	iss >> this->_method >> this->_path >> this->_protocol;
}

void AMethod::parseHeaderFields(void)
{
	std::deque<std::string>::const_iterator it = this->_linesBuffer.begin();
	std::deque<std::string>::const_iterator end = this->_linesBuffer.end();

	std::string key;
	std::string value;

	size_t	pos = 0;
	while (it != end)
	{
		pos = (*it).find(": ");
		key = (*it).substr(0, pos);
		value = (*it).substr(pos + 2, (*it).size() - pos - 2);
		this->_linesBuffer.pop_front();
		this->_headerFields.insert(std::pair<std::string, std::string>(key, value));
		++it;
		if (*it == "") // body가 존재할 경우 멈춤
		{
			this->_linesBuffer.pop_front();
			break ;
		}
	}
}

const std::string &AMethod::getResponse(void) const
{
	return (this->_response);
}

bool AMethod::getResponseFlag(void) const
{
	return (this->_responseFlag);
}
