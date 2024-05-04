#ifndef SOCKET_UDP_H
#define SOCKET_UDP_H

#include "../socket/socket.h"

class SocketUDP : public Socket{
    public:
        SocketUDP(const SocketAddress &socket_address);

        SocketUDP(int sock_fd);

        ~SocketUDP();

        //надо знать куда хотим
        void send(const struct sockaddr_in &address, const std::vector<char> &message);

        //определяем откуда пришло
        std::vector<char> receive(struct sockaddr_in &address);
    private:
        SocketUDP(const SocketUDP &) = delete;
};

#endif