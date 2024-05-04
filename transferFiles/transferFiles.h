#ifndef TRANSFER_FILES_H
#define TRANSFER_FILES_H

#include "../directory/directory.h"
#include "../messagePack/messagePack.h"
#include "../messageUnPack/messageUnPack.h"
#include "../socketTcp/socketTCP.h"
#include "../socketTcpServer/socketTcpServer.h"

std::vector<char> create_bytes_of_file(const std::filesystem::path &path);

void send_full_message(const SocketTcpServer &socket, MessagePack &message);

void send_files(const SocketTcpServer &socket, const Directory& dir);

std::vector<char> receive_full_message(const SocketTCP &socket, MessageUnpack &message);

void receive_files(const SocketTCP &socket, const std::filesystem::path &path);

std::string get_list_files(const std::string &path);

#endif