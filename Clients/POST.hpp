#ifndef POST_HPP
#define POST_HPP

#include "AMethod.hpp"

class POST : public AMethod
{
	private :
		std::string body;
	public :
		POST() {};
        POST(std::string &request) : AMethod(request) {}
		~POST() {};

		void parseRequest() {}
		void createResponse() {}
};

#endif 
