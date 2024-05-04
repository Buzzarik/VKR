#include "socketException.h"

SocketException::SocketException(const std::string &error)
        :_error(error){}

const char *SocketException::what() const throw(){
    return _error.c_str();
}

SocketException::~SocketException()throw(){}