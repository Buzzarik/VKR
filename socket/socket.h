#ifndef SOCKET_H
#define SOCKET_H

#include "../socketAddress/socketAddress.h"
#include <netinet/in.h>
#include <arpa/inet.h>

#define RECV_SIZE 1024

class Socket{
    public:
        Socket(const SocketAddress &socket_address);

        Socket(int socket_fd);

        virtual ~Socket();

        SocketAddress get_socket_address() const;

        void close();
        
        bool is_open() const;
    protected:
        Socket(const Socket &) = delete;

        int _sock_fd;
        bool _is_open;
        struct sockaddr_in _address; //это куда будем отправлять
};

#endif