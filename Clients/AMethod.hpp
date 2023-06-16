#ifndef AMethod_HPP
# define AMethod_HPP

#include <string>
#include <sstream>
#include <map>
#include <deque>

class AMethod
{
	protected:
		std::string _request;
		std::string _response;
		bool		_responseFlag; // when generating response is done, set this flag to true

		std::string _method;
		std::string _path;
		std::string _protocol;

		std::deque<std::string> _linesBuffer;
		std::map<std::string, std::string> _headerFields;

		// CRLF 기준으로  스플릿하는  함수 , _requestVector 을 initializing 해준다.
			//-> 각 줄마다 vectorize 하기
		void splitLinesByCRLF(void); // 매개변수 지웠어 헷
		// 요청라인 공백 기준으로 파싱하는 함수
			//-> method, path, protocol 멤버함수를 초기화시켜준다  //RFC메뉴얼 기준으로...추후논의
		void parseRequestLine(void);
		// ": " 으로 스플릿하는 함수 -> POST 요청에다만 구현하면되니깐 virtual
			//-> vectorize 되어있는 컨테이너 linear 탐색후 매퍼에다가 파싱시켜주기
		void parseHeaderFields(void);
	public:
		AMethod();
		AMethod(std::string &request);
		virtual ~AMethod();

		//요청 들어온 것에 대해서 응답을 생성한다.
		virtual void createResponse(void) = 0;

		const std::string	&getResponse(void) const;
		bool				getResponseFlag(void) const;

};

#endif
