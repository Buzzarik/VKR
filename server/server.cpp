#include "../socketUdpServer/socketUDPServer.h"
#include <iostream>
#include <arpa/inet.h>
#include <unordered_map>
#include <unordered_set>
#include <chrono>


bool operator==(const sockaddr_in &addr1, const sockaddr_in &addr2){
    return addr1.sin_addr.s_addr == addr2.sin_addr.s_addr && addr1.sin_family == addr2.sin_family && addr1.sin_port == addr2.sin_port;
}

bool operator<(const sockaddr_in &addr1, const sockaddr_in &addr2){
    return std::tie(addr1.sin_family, addr1.sin_addr.s_addr, addr1.sin_port) < std::tie(addr2.sin_family, addr2.sin_addr.s_addr, addr2.sin_port);
}

static size_t identifier = 1;

int main(int argc, char **argv){

    std::string ip = "127.0.0.1";
    uint16_t port = 9000;

    //условно для проверки соединения
    std::vector<uint16_t> portConnectPeers = {6100, 6200};
    size_t index = 0;

try{
    //создаеи сокет UDP сервера и привязывем его к адресу
    SocketAddress addressServer(ip, port);
    SocketUdpServer socketServer(addressServer);

    //DEBUG
    std::cout << "Я UDP сервер с адресом: " << socketServer.get_socket_address() << std::endl;

    std::unordered_map<std::string, std::chrono::steady_clock::time_point> clients_time;
    std::unordered_map<std::string, sockaddr_in> clients;
    std::unordered_map<std::string, std::unordered_set<std::string>> clients_connect;

    bool notClients = false; //чтобы сервер отключался, когда нет ни единого клиента
    while (!notClients){ //пока будет бесконечный цикл

        sockaddr_in client;
        std::vector<char> buf = socketServer.receive(client);

        bool isFirst = true;
        for (const auto& [str, addr] : clients){
            if (addr == client){
                isFirst = false;
                break;
            }
        }

        if (isFirst){
            clients_time["client" + std::to_string(identifier)] = std::chrono::steady_clock::now();
            clients["client" + std::to_string(identifier++)] = client;
            std::string response = "Привет клиент!";
            socketServer.send(client, {response.begin(), response.end()});
        }
        else{
            std::string request = std::string(buf.begin(), buf.end());
            std::string response;

            //при каждом приходе сообщения обновляем наш список клиентов во всех списках
            const auto endTime = std::chrono::steady_clock::now(); //проверяем время
            for (const auto& [clientId, time] : clients_time){
                if (const std::chrono::duration<double> elapsed{ endTime - time}; elapsed.count() > 15){ //значит произошел сбой со стороны клиента
                    //удаляем отовсюду с выводом всякий сообщений
                    const auto& to_clientId = clients_connect[clientId];
                    for (const auto& peer_to_clientId : to_clientId){
                        sockaddr_in addr = clients[peer_to_clientId];
                        response = "такой клиент занят или не в сети";
                        socketServer.send(addr, {response.begin(), response.end()});  
                    }
                    clients_connect.erase(clientId);
                    clients.erase(clientId);
                    clients_time.erase(clientId);
                }
            }
            
            if (request.starts_with("list")){ //отправляем список всех клиентов или пишем, что их нет
                if (clients.size() > 1){
                    for (const auto& [id, addr] : clients){
                        if (addr != client){
                            response += id + "\n";
                        }
                    }
                }
                else{
                    response = "Список пуст";
                }
                socketServer.send(client, {response.begin(), response.end()});
            }
            else if (request.starts_with("client")){

                if (!clients.contains(request)){
                    response = "Такого клиента нет. Введите команду list, чтобы узнать подключенных клиентов";
                    socketServer.send(client, {response.begin(), response.end()});
                }
                else{ 
                    std::string clientId;
                    for (const auto& [id, addr] : clients){
                        if (addr == client){
                            clientId = id;
                            break;
                        }
                    }
                    if (clientId == request){
                        response = "Такого клиента нет. Вы не можете подключится к самому себе";
                        socketServer.send(client, {response.begin(), response.end()});
                    }
                    if (!(clients_connect.contains(clientId) && clients_connect[clientId].contains(request))){
                        clients_connect[request].insert(clientId);
                    }
                    else{
                        std::string portPeers = std::to_string(portConnectPeers[index]);
                        index++; index %= portConnectPeers.size();
                        std::string portIn = "in " + portPeers;
                        std::string portOut = "out " + portPeers;

                        sockaddr_in peer = clients[request];
                        socketServer.send(peer, {portIn.begin(), portIn.end()});

                        socketServer.send(client, {portOut.begin(), portOut.end()});


                        //отключаем его самого + отправляем сообщения другим пирам, что он занят или не в сети
                        const auto& to_clientId = clients_connect[clientId];
                        for (const auto& peer_to_clientId : to_clientId){
                            sockaddr_in addr = clients[peer_to_clientId];
                            response = "такой клиент занят или не в сети";
                            socketServer.send(addr, {response.begin(), response.end()});  
                        }
                        clients_connect.erase(clientId);

                        const auto& to_peerId = clients_connect[request];
                        for (const auto& peer_to_clientId : to_peerId){
                            sockaddr_in addr = clients[peer_to_clientId];
                            response = "такой клиент занят или не в сети";
                            socketServer.send(addr, {response.begin(), response.end()});  
                        }
                        clients_connect.erase(clientId);

                        //теперь удалить их из всех соединений как peer, так и client
                        clients.erase(clientId);
                        clients.erase(request);
                        clients_time.erase(clientId);
                        clients_time.erase(request);
                    }
                }
            }
            else if (request.starts_with("exit")){  //удаляем клиента из списка
                std::string clientId;
                for (const auto& [id, addr] : clients){
                    if (addr == client){
                        clientId = id;
                        break;
                    }
                }
                //отключаем его самого + отправляем сообщения другим пирам, что он занят
                const auto& to_clientId = clients_connect[clientId];
                for (const auto& peer_to_clientId : to_clientId){
                    sockaddr_in addr = clients[peer_to_clientId];
                    response = "такой клиент занят или не в сети";
                    socketServer.send(addr, {response.begin(), response.end()});  
                }
                clients_connect.erase(clientId);
                clients.erase(clientId);
                clients_time.erase(clientId);

                response = "До новых встреч!";
                socketServer.send(client, {response.begin(), response.end()});
            }
            else if (request.starts_with("alive")){ 
                std::string clientId;
                for (const auto& [id, addr] : clients){
                    if (addr == client){
                        clientId = id;
                        break;
                    }
                }
                clients_time[clientId] = std::chrono::steady_clock::now();
            }
            else{ //если некорректное сообщение
                response = "Некорректная команда. Для справки напишите help";
                socketServer.send(client, {response.begin(), response.end()});
            }
        }
        if (clients.empty()){
            notClients = true; //чтобы не перезапускать заново комп, а снова подключаться к этому адресу
        }
    }
        
    socketServer.close();
    
}catch(std::exception &e){
    std::cout << e.what() << std::endl;
}
    return 0;
}