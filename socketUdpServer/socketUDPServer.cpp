#include "socketUDPServer.h"
#include "../socketException/socketException.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

SocketUdpServer::SocketUdpServer(const SocketAddress &socket_address)
        :SocketUDP(socket_address){
            
    int status = ::bind(_sock_fd, (const struct sockaddr *)&_address, sizeof(_address));

    if (status < 0){
        char ip_str[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(_address.sin_addr), ip_str, INET_ADDRSTRLEN);
        throw SocketException("Неудачное привязка сокета UDP к адресу " + std::string(ip_str) + 
        ":" + std::to_string(ntohs(_address.sin_port)) +
        ". Причина: " + strerror(errno));
    }

}

SocketUdpServer::SocketUdpServer(int sock_fd)
    :SocketUDP(sock_fd){

    int status = ::bind(_sock_fd, (const struct sockaddr *)&_address, sizeof(_address));

    if (status < 0){
        char ip_str[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(_address.sin_addr), ip_str, INET_ADDRSTRLEN);
        throw SocketException("Неудачное привязка сокета UDP к адресу " + std::string(ip_str) + 
        ":" + std::to_string(ntohs(_address.sin_port)) +
        ". Причина: " + strerror(errno));
    }

}