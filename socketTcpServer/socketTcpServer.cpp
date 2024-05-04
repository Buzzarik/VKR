#include "socketTcpServer.h"
#include "../socketException/socketException.h"
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>


SocketTcpServer::SocketTcpServer(const SocketAddress &socket_address)
        :Socket(socket_address){
    _sock_fd = ::socket(AF_INET, SOCK_STREAM, 0);
    const int enable = 1;
    setsockopt(_sock_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));
    bind();
    listen();
}

SocketTcpServer::SocketTcpServer(int sock_fd)
        :Socket(sock_fd){
    bind();
    listen();
}

void SocketTcpServer::send(const std::vector<char> &message) const{
    assert(_is_open);
    if (message.empty())
        return;
    ssize_t bytes = ::send(_sock_client, &message[0], message.size(), 0);
    if (bytes < 0 || static_cast<const unsigned long>(bytes) != message.size()){
        throw SocketException("Ошибка в отправке сообщения через TCP. Передано " +
        std::to_string(bytes) + " из " + std::to_string(message.size()) + ". Причина: "
                + std::string(strerror(errno)));
    }
}

std::vector<char> SocketTcpServer::receive() const{
    std::vector<char> received_data(RECV_SIZE, '\0');
    ssize_t bytes = ::recv(_sock_client, &received_data[0], RECV_SIZE, 0);

    if (bytes < 0){
        throw SocketException("Ошибка в отправке сообщения через TCP. Причина: "
                + std::string(strerror(errno)));
    }
    //убираем ненужные элементы, если сообщение меньше, чем RECV_SIZE
    received_data.resize(bytes);
    return received_data;
}


void SocketTcpServer::bind(){
    assert(!_is_open);
    int status = ::bind(_sock_fd, (struct sockaddr *)&_address, sizeof(_address));

    if (status < 0){
        char ip_str[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(_address.sin_addr), ip_str, INET_ADDRSTRLEN);
        throw SocketException("Неудачное привязка по TCP к адресу " + std::string(ip_str) + 
        ":" + std::to_string(ntohs(_address.sin_port)) +
        ". Причина: " + strerror(errno));
    }

}

void SocketTcpServer::listen(){
    assert(!_is_open);
    int status = ::listen(_sock_fd, 5); // пускай пока будет 5
    if (status < 0){
        char ip_str[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(_address.sin_addr), ip_str, INET_ADDRSTRLEN);
        throw SocketException("Неудачное создание очереди по TCP соединению к адресу " + std::string(ip_str) + 
        ":" + std::to_string(ntohs(_address.sin_port)) +
        ". Причина: " + strerror(errno));
    }
    _is_open = true;
}

void SocketTcpServer::accept(){
    assert(_is_open);
    _sock_client = ::accept(_sock_fd, NULL, NULL);
    if (_sock_client < 0){
        throw SocketException("Неудачное соединение по TCP. Причина: " + std::string(strerror(errno)));
    }
}


void SocketTcpServer::close(){
    if (_is_open){
        if (::close(_sock_client) < 0){
            throw SocketException("Ошибка при закрытие сокета. Причина: "
                    + std::string(strerror(errno)));
        }
        Socket::close();
        _is_open = false;
    }
}

SocketTcpServer::~SocketTcpServer(){
    if (_is_open){
        close();
        _is_open = true; //чтобы корректно сработал десткрутор socket
    }
}