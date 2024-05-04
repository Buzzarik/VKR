#include "socketAddress.h"


SocketAddress::SocketAddress(const std::string &ip, uint16_t port)
        :_port(port), _ip(ip){}

std::string SocketAddress::ip() const{
    return _ip;
}

uint16_t SocketAddress::port() const{
    return _port;
}

std::ostream &operator<<(std::ostream &os, const SocketAddress &address){
    os << address.ip() << ":" << address.port();
    return os;
}

bool operator==(const SocketAddress &addr1, const SocketAddress &addr2){
    return addr1.ip() == addr2.ip() && addr1.port() == addr2.port();
}
