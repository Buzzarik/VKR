#ifndef SOCKET_ADDRESS_H
#define SOCKET_ADDRESS_H

#include <iostream>
#include <string>

class SocketAddress{
    public:
        SocketAddress(const std::string &ip, uint16_t port);

        std::string ip() const;
        uint16_t port() const;

    private:
        uint16_t _port;
        std::string _ip;
};

std::ostream &operator<<(std::ostream &os, const SocketAddress &address);
bool operator==(const SocketAddress &addr1, const SocketAddress &addr2);

#endif