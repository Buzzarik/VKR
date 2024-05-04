#ifndef INTERFACE_H
#define INTERFACE_H

#include "../transferFiles/transferFiles.h"

#define MAIN_DIR_SERVER "/Users/evgeniy/Document/Programming/C++/P2P.2.0/serverFiles"
#define MAIN_DIR_CLIENT "/Users/evgeniy/Document/Programming/C++/P2P.2.0/clientFiles"

void print(const std::vector<char> &message);

void response_files(const SocketTcpServer &socket, const std::string &filename, const Directory &directory);

void interface_receiver(const SocketTcpServer &socket);

void printHelpForServer();

void printHelpForClient();

void interface_sender(const SocketTCP &socket);

#endif