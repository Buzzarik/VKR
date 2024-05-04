#ifndef SOCKET_UDP_SERVER_H
#define SOCKET_UDP_SERVER_H

#include "../socketUdp/socketUDP.h"

class SocketUdpServer : public SocketUDP{
    public:
    SocketUdpServer(const SocketAddress &socket_address);

    SocketUdpServer(int sock_fd);
};

#endif