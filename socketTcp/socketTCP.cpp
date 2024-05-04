#include "socketTCP.h"
#include "../socketException/socketException.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

SocketTCP::SocketTCP(const SocketAddress &socket_address)
        :Socket(socket_address){
        _sock_fd = ::socket(AF_INET, SOCK_STREAM, 0);
        const int enable = 1;
        setsockopt(_sock_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));
        connect();
}

SocketTCP::SocketTCP(int sock_fd)
        : Socket(sock_fd){
            connect();
}

SocketTCP::~SocketTCP(){}

void SocketTCP::send(const std::vector<char> &message) const{
    assert(_is_open);
    if (message.empty())
        return;
        
    ssize_t bytes = ::send(_sock_fd, &message[0], message.size(), 0);
    if (bytes < 0 || static_cast<const unsigned long>(bytes) != message.size()){
        throw SocketException("Ошибка в отправке сообщения через TCP. Передано " +
        std::to_string(bytes) + " из " + std::to_string(message.size()) + ". Причина: "
                + std::string(strerror(errno)));
    }
}

std::vector<char> SocketTCP::receive() const{
    assert(_is_open);
    std::vector<char> received_data(RECV_SIZE, '\0');
    ssize_t bytes = ::recv(_sock_fd, &received_data[0], RECV_SIZE, 0);

    if (bytes < 0){
        throw SocketException("Ошибка в отправке сообщения через TCP. Причина: "
                + std::string(strerror(errno)));
    }
    //убираем ненужные элементы, если сообщение меньше, чем RECV_SIZE
    received_data.resize(bytes);
    return received_data;
}

void SocketTCP::connect(){
    //если уже есть соедниение?
    assert(!_is_open);
    int status = ::connect(_sock_fd, (struct sockaddr *)&_address, sizeof(_address));
    if (status < 0){
        char ip_str[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(_address.sin_addr), ip_str, INET_ADDRSTRLEN);
        throw SocketException("Неудачное соединение по TCP к адресу " + std::string(ip_str) + 
        ":" + std::to_string(ntohs(_address.sin_port)) +
        ". Причина: " + strerror(errno));
    }
    _is_open = true;
}