#ifndef SOCKET_EXCEPTION_H
#define SOCKET_EXCEPTION_H

#include <string>
#include <exception>

class SocketException : public std::exception{
    public:
        SocketException(const std::string &error);

        virtual const char* what() const throw();
        virtual ~SocketException() throw();
    private:
        std::string _error;
};

#endif