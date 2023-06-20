#ifndef DELETE_HPP
#define DELETE_HPP

#include "AMethod.hpp"

class DELETE : public AMethod
{
    private :
    public :
        DELETE() {};
        DELETE(std::string &request) : AMethod(request) {}
        ~DELETE() {};

        void parseRequestMessage() {}
		void createResponseMessage() {}
};

#endif 
