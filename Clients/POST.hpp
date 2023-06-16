#ifndef POST_HPP
#define POST_HPP

#include "AMethod.hpp"

class POST : public AMethod
{
	private :
	public :
		POST() {};
		~POST() {};

		void parseRequest(std::string _request, std::string _method, \
		std::map<std::string, std::string> _headMapper) {}
};

#endif 