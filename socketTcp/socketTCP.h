#ifndef SOCKET_TCP_H
#define SOCKET_TCP_H

#include "../socket/socket.h"

class SocketTCP : public Socket{
    public:
        SocketTCP(const SocketAddress &socket_address);

        SocketTCP(int sock_fd);

        ~SocketTCP();

        void send(const std::vector<char> &message) const;

        std::vector<char> receive() const;

    protected:
        void connect();
};

#endif