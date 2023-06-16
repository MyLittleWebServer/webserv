#ifndef GET_HPP
#define GET_HPP

#include "AMethod.hpp"

class GET : public AMethod
{
    private :
    public :
        GET() {}
        GET(std::string &request) : AMethod(request) {}
        ~GET() {}

        void parseRequest(std::string _request, std::string _method, \
		std::map<std::string, std::string> _headMapper) {}
};

#endif 