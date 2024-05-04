#include "transferFiles.h"
#include <fstream>

std::vector<char> create_bytes_of_file(const std::filesystem::path &path){
    std::fstream file;
    char* bytes = nullptr;
    uint32_t size = static_cast<uint32_t>(std::filesystem::file_size(path));
    file.open(path, std::ios_base::in | std::ios_base::binary);
    if (file.is_open()){
        bytes = new char[size];
        file.read(bytes, size);
        std::vector<char> buf(bytes, bytes + size);
    }
    file.close();
    if (bytes == nullptr)
        return {};
    auto buf = std::vector<char>(bytes, bytes + size);
    delete[] bytes;
    return buf;
}

void send_full_message(const SocketTcpServer &socket, MessagePack &message){
    while (!message.is_read()){
        socket.send(message.get_pack_fragment());
        message.next_fragment();
    }
    message.reload();
}

std::vector<char> receive_full_message(const SocketTCP &socket, MessageUnpack &message){
    while (!message.is_write()){
        message.add_fragment(socket.receive());
    }
    return message.get_buffer();
}

void send_files(const SocketTcpServer &socket, const Directory& dir){
    MessagePack pack;
    std::string fileOrDirName = dir.get_directory().filename().string();
    if (dir.is_directory()) {
        //передача названия директории
        pack.set_type(MessageType::TYPE_START_TRANSFER_DIRECTORY);
        pack.set_message({ fileOrDirName.begin(), fileOrDirName.end() });

        send_full_message(socket, pack);

        //передача содержания директории
        for (const auto& entry : std::filesystem::directory_iterator(dir.get_directory())) {
            send_files(socket, Directory(entry.path()));
        }

        //конец передачи директории
        pack.set_type(MessageType::TYPE_FINISH_TRANSFER_DIRECTORY);
        pack.set_message({ fileOrDirName.begin(), fileOrDirName.end() });

        send_full_message(socket, pack);
    }
    else {
        //начала передачи файла (название)
        pack.set_type(MessageType::TYPE_FILENAME);
        pack.set_message({ fileOrDirName.begin(), fileOrDirName.end() });

        send_full_message(socket, pack);

        //передача самого файла
        std::vector<char> bytes = create_bytes_of_file(dir.get_directory());
        pack.set_type(MessageType::TYPE_TRANSFER_FILE);
        pack.set_message(bytes);

        send_full_message(socket, pack);
    }
}

void receive_files(const SocketTCP &socket, const std::filesystem::path &path){
    MessageUnpack unpack;
    std::vector<char> buf;
    while (unpack.get_type() != MessageType::TYPE_FINISH_TRANSFER_FILES){

        if (unpack.get_type() == MessageType::TYPE_START_TRANSFER_DIRECTORY){
            //получаем все фрагменты (название)
            buf = receive_full_message(socket, unpack);
            
            //создаем директорию в path лежит путь вместе с названием директории
            std::string directoryPath = path.string() + "/" + std::string(buf.begin(), buf.end());
            std::filesystem::create_directory(directoryPath);

            //теперь заходим рекурсивно и пытаемся принять содержимое директории
            receive_files(socket, directoryPath);
        }
        else if (unpack.get_type() == MessageType::TYPE_FINISH_TRANSFER_DIRECTORY){
            return; //по сути выход из рекурсии
        }
        else if (unpack.get_type() == MessageType::TYPE_FINISH_TRANSFER_FILES){
            return; //по сути выход из главной функции (уже должны все рекурсии пройти :))
        }
        else if (unpack.get_type() == MessageType::TYPE_FILENAME){
            //получаем все фрагменты (название)
            buf = receive_full_message(socket, unpack);

            //пришло название файла, надо создать и открыть + сразу следующий прием его содержимого
            std::string filePath = path.string() + "/" + std::string(buf.begin(), buf.end());
            std::ofstream file(filePath, std::ios::binary | std::ios::out);

            //забираем 1 данные для записи в файл
            unpack.set_message(socket.receive());

            //забираем все остальное
            buf = receive_full_message(socket, unpack);

            //записываем в наш файл
            file.write(buf.data(), buf.size());

            //закрываем файл
            file.close();
        }
        
        //опять читаем сообщения
        unpack.set_message(socket.receive());
    }
}

std::string get_list_files(const std::string &path){
    Directory directory(path);
    return directory.get_list_files();
}
