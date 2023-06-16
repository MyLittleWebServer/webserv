#ifndef DELETE_HPP
#define DELETE_HPP

#include "AMethod.hpp"

class DELETE : public AMethod
{
    private :
    public :
        DELETE() {};
        ~DELETE() {};

        void parseRequest(std::string _request, std::string _method, \
		std::map<std::string, std::string> _headMapper) {}
};

#endif 