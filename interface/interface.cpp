#include "interface.h"
#include "../socketException/socketException.h"
#include <iostream>

void print(const std::vector<char> &message){
    for (char c : message){
        std::cout << c;
    }
    std::cout << std::endl;
}

void response_files(const SocketTcpServer &socket, const std::string &filename, const Directory &directory){
    MessagePack pack;
    std::string response;

    auto findFiles = directory.find_files(filename);

    if (findFiles.empty()){
        response = "Такого файла нет";
        pack.set_message({response.begin(), response.end()});
        pack.set_type(MessageType::TYPE_INCORRECT_FILENAME);
        socket.send(pack.get_pack_fragment()); 
        pack.reload();
    }
    else{
        pack.set_message({'1', '2'});
        pack.set_type(MessageType::TYPE_START_TRANSFER_FILES);
        socket.send(pack.get_pack_fragment());
        for (const auto& path : findFiles){
            Directory dir(path);
            send_files(socket, dir);
        }
        //выдаем сообщение, что передача файлов закончилась
        pack.set_type(MessageType::TYPE_FINISH_TRANSFER_FILES);
        pack.set_message({'3', '4'}); //условно
        socket.send(pack.get_pack_fragment()); //что все передано
    }
}

void interface_receiver(const SocketTcpServer &socket){
    std::string request = "unknown";
    std::string response;
    std::vector<char> buf;
    MessageUnpack unpack;
    MessagePack pack;

    while (unpack.get_type() != MessageType::TYPE_EXIT_CONNECT){
        unpack.add_fragment(socket.receive());
        buf = unpack.get_buffer();

        request = std::string(buf.begin(), buf.end());

        //if (request.starts_with("list_files")){
        if (unpack.get_type() == MessageType::TYPE_LIST_FILES){
            response = get_list_files(MAIN_DIR_SERVER);
            MessagePack message(MessageType::TYPE_LIST_FILES, {response.begin(), response.end()});
            send_full_message(socket, message);
            unpack.clear();
        }
        else if (unpack.get_type() == MessageType::TYPE_LOAD_FILES){
            //выявляем название файла или директорию
            buf = unpack.get_buffer();
            std::string filename(buf.begin(), buf.end());

            if (filename.size() < 5){ //если ничего не написали
                response = "Не указан файл для скачивания";
                pack.set_message({response.begin(), response.end()});
                pack.set_type(MessageType::TYPE_INCORRECT_FILENAME);
                socket.send(pack.get_pack_fragment());
                pack.reload();
            }
            else{
                filename = filename.substr(5);
                Directory directory(MAIN_DIR_SERVER);
                response_files(socket, filename, directory);
            }
            unpack.clear();
        }
        else if (unpack.get_type() == MessageType::TYPE_EXIT_CONNECT){
                response = "Пока, пока...";
                pack.set_message({response.begin(), response.end()});
                pack.set_type(MessageType::TYPE_EXIT_CONNECT);
                socket.send(pack.get_pack_fragment());
            }
            else{
                response = "Некорректное сообщение, напишите help для справки";
                pack.set_message({response.begin(), response.end()});
                pack.set_type(MessageType::TYPE_ERROR_ENTER);
                socket.send(pack.get_pack_fragment());
                unpack.clear();
            }
    }
}

void printHelpForServer(){
    std::cout << "1 - list - запросить список пиров\n" 
        << "2 - client[ID] - запрос на связывание c клиентом из списка\n"
        << "Чтобы можно было связаться с другим клиентом, нужно подтверждение обоих клиентов\n"
        << "3 - exit"
        << std::endl;
}

void printHelpForClient(){
    std::cout << "1 - list_files - выдать список файлов для скачивания\n"
        << "2 - load <имя файла/директория> - скачать файл или всю директорию\n"
        << "3 - exit_connect - отключится от клиента\n"
        << "4 - help"
        << std::endl;
}

void interface_sender(const SocketTCP &socket){
    std::string request = "unknown";
    std::string response;
    std::vector<char> buf;
    while (!request.starts_with("exit_connect")){
        std::cout << std::endl;
        printf("Нужна помощь - напишите 'help'\n");
        std::getline(std::cin, request);

        //request = answers[index++];
        MessageUnpack unpack;
        MessagePack pack;
        if (request.starts_with("help")){
            printHelpForClient();
        }
        else{
            if (request.starts_with("list_files")){
                pack.set_message({request.begin(), request.end()});
                pack.set_type(MessageType::TYPE_LIST_FILES);
                socket.send(pack.get_pack_fragment());

                //это, чтобы можно было собирать сообщение по фрагментам
                unpack.add_fragment(socket.receive());
                if (unpack.get_type() != MessageType::TYPE_LIST_FILES){
                    throw SocketException("при запросе списка получили не то сообщение!");
                }
                //получаем остальные фрагменты сообщения
                buf = receive_full_message(socket, unpack);
            }
            else if (request.starts_with("load")){

                pack.set_message({request.begin(), request.end()});
                pack.set_type(MessageType::TYPE_LOAD_FILES);
                socket.send(pack.get_pack_fragment());

                unpack.add_fragment(socket.receive());
                if (unpack.get_type() == MessageType::TYPE_INCORRECT_FILENAME){
                    buf = unpack.get_buffer();
                }
                else if (unpack.get_type() == MessageType::TYPE_START_TRANSFER_FILES){
                    receive_files(socket, MAIN_DIR_CLIENT);
                    std::string signal = "Файлы скачаны!";
                    buf = {signal.begin(), signal.end()};
                }
                else{
                    throw SocketException("при запросе на скачиавание получили не то сообщение!");
                }
            }
            else if (request.starts_with("exit_connect")){
                pack.set_message({request.begin(), request.end()});
                pack.set_type(MessageType::TYPE_EXIT_CONNECT);
                socket.send(pack.get_pack_fragment());

                unpack.add_fragment(socket.receive());
                if (unpack.get_type() != MessageType::TYPE_EXIT_CONNECT){
                    throw SocketException("при запросе на выход получили не то сообщение!");
                }
                buf = receive_full_message(socket, unpack);
            }
            else{ //если что-то другое (там просто ответ - некорректный ввод)
                pack.set_message({request.begin(), request.end()});
                pack.set_type(MessageType::TYPE_ERROR_ENTER);
                socket.send(pack.get_pack_fragment());

                unpack.add_fragment(socket.receive());
                if (unpack.get_type() != MessageType::TYPE_ERROR_ENTER){
                    throw SocketException("при некорректном запросе пришло не то сообщение!");
                }
                buf = receive_full_message(socket, unpack);
            }
            print(buf); //оставлено для проверки, по сути response
        }
        unpack.clear();
    }
}