#ifndef AMethod_HPP
# define AMethod_HPP

#include <string>
#include <sstream>
#include <map>
#include <vector>

class AMethod
{
	protected:
		std::string _method;
		std::string _path;
		std::string _protocol;

		std::string _request;
		std::string _response;
		bool		_responseFlag; // when generating response is done, set this flag to true
		std::vector<std::string> _requestVector;
		std::map<std::string, std::string> _headMapper;

		// CRLF 기준으로  스플릿하는  함수 , _requestVector 을 initializing 해준다.
			//-> 각 줄마다 vectorize 하기
		void requestVector(std::string _request, std::vector<std::string> _requestVector);
		// 요청라인 공백 기준으로 파싱하는 함수
			//-> method, path, protocol 멤버함수를 초기화시켜준다  //RFC메뉴얼 기준으로...추후논의
		void requestParser(std::vector<std::string> _requestVector);
		// ": " 으로 스플릿하는 함수 -> POST 요청에다만 구현하면되니깐 virtual
			//-> vectorize 되어있는 컨테이너 linear 탐색후 매퍼에다가 파싱시켜주기
		virtual void requestSplitByColon(std::vector<std::string> _requestVector);
	public:
		AMethod();
		AMethod(std::string &request);
		virtual ~AMethod();

		//요청 들어온 것에 대해서 응답을 생성한다.
		virtual void responseCreation(void) = 0;


		const std::string	&getResponse(void) const;
		bool				getResponseFlag(void) const;

};

#endif