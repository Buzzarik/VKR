#include "socketUDP.h"
#include "../socketException/socketException.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <assert.h>


SocketUDP::SocketUDP(const SocketAddress &socket_address)
        :Socket(socket_address){
    _sock_fd = ::socket(AF_INET, SOCK_DGRAM, 0);
    const int enable = 1;
    setsockopt(_sock_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));
    _is_open = true;
}

SocketUDP::SocketUDP(int sock_fd)
        :Socket(sock_fd){
    _is_open = true;
} //тоже не надо ничего, уже соедниение есть

SocketUDP::~SocketUDP() {} //по сути нового от Socket нет, чтобы очищать не по умолчанию

void SocketUDP::send(const struct sockaddr_in &address, const std::vector<char> &message){
    assert(_is_open);
    if (message.empty())
        return;
    socklen_t len = sizeof(address);
    ssize_t bytes = ::sendto(_sock_fd, &message[0], message.size(), 0,
            (const struct sockaddr *)&address, len);
        
    if (bytes < 0 || static_cast<const unsigned long>(bytes) != message.size()){
        throw SocketException("Ошибка в отправке сообщения через UDP. Передано " +
        std::to_string(bytes) + " из " + std::to_string(message.size()) + ". Причина: "
                + std::string(strerror(errno)));
    }
}

//делаем просто обычный запрос на длину RECV_SIZE, если будет больше сообщение, то за это будет отвечать уже 
//буфер со стороны peer и спецификация сообщения о том, сколько еще принемать
std::vector<char> SocketUDP::receive(struct sockaddr_in &address){
    assert(_is_open);
    socklen_t len = sizeof(address);
    std::vector<char> received_data(RECV_SIZE, '\0');


    ssize_t bytes = recvfrom(_sock_fd, &received_data[0], RECV_SIZE, MSG_WAITALL,
            (struct sockaddr *)&address, &len);
    
    if (bytes < 0){
        throw SocketException("Ошибка в получении сообщения через UDP. Причина: "
                + std::string(strerror(errno)));
    }

    //убираем ненужные элементы, если сообщение меньше, чем RECV_SIZE
    received_data.resize(bytes);
    return received_data;
}

