#include "socket.h"
#include "../socketException/socketException.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <assert.h>


Socket::Socket(const SocketAddress &socket_address)
        :_is_open(false){
    memset(&_address, 0, sizeof(_address));
    _address.sin_family = AF_INET;
    _address.sin_port = htons(socket_address.port());
    //может иметь ошибку в преобразовании адреса
    inet_pton(AF_INET, socket_address.ip().c_str(), &(_address.sin_addr));
}

//не используем socket и connect, так уже должен быть уже создано соедниение
Socket::Socket(int sock_fd)
    : _sock_fd(sock_fd), _is_open(false){
    socklen_t len = sizeof(_address);
    getpeername(_sock_fd, (struct sockaddr *)&_address, &len);
}

void Socket::close(){
    assert(_is_open);
    if (::close(_sock_fd) < 0){
        throw SocketException("Ошибка при закрытие сокета. Причина: "
                + std::string(strerror(errno)));
    }
    _is_open = false;
}

bool Socket::is_open() const{
    return _is_open;
}

Socket::~Socket(){
    if (_is_open){
        ::close(_sock_fd);
        _is_open = false;
    }
}

SocketAddress Socket::get_socket_address() const{
    char ip_str[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(_address.sin_addr), ip_str, INET_ADDRSTRLEN);

    return SocketAddress(ip_str, ntohs(_address.sin_port));
}
