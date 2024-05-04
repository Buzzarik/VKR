#ifndef SOCKET_TCP_SERVER_H
#define SOCKET_TCP_SERVER_H

#include "../socket/socket.h"

class SocketTcpServer : public Socket{
    public:
        SocketTcpServer(const SocketAddress &socket_address);

        SocketTcpServer(int sock_fd);

        void accept();

        ~SocketTcpServer();

        void send(const std::vector<char> &message) const;

        std::vector<char> receive() const;

        void close();
    protected:
        void bind();
        void listen();
    private:
        int _sock_client;
};

#endif