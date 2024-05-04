#include "../interface/interface.h"
#include "../socketUdp/socketUDP.h"
#include "../socketException/socketException.h"
#include <iostream>
#include <arpa/inet.h>
#include <thread>
#include <mutex>

std::mutex mutex;

int main(int argc, char **argv){

    std::string ip = "127.0.0.1";
    uint16_t port = 9000;

try{
    SocketAddress addressServer(ip, port);
    SocketUDP socketClientUdp(addressServer);

    std::cout << "Я клиент и буду подключаться к UDP серверу на адрес: " << socketClientUdp.get_socket_address() << std::endl;

    //эта вставка нужна, чтобы назначит куда отпрвлять
    struct sockaddr_in address;
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    inet_pton(AF_INET, ip.c_str(), &(address.sin_addr));
    ///////////////

    std::string message = "client";
    std::vector<char> buf;
    std::string request, response;


    //запускаем поток keep_alive и ставим флаг для его сброса!
    bool isStopAlive = false;
    std::thread thr_keep_alive([&isStopAlive, &socketClientUdp, address](){
        auto start {std::chrono::steady_clock::now()};
        while (!isStopAlive){
            auto end {std::chrono::steady_clock::now()};
            if (const std::chrono::duration<double> elapsed {end - start}; elapsed.count() > 10){
                const std::lock_guard<std::mutex> lock(mutex); 
                //отправка alive
                std::string request = "alive";
                socketClientUdp.send(address, {request.begin(), request.end()});
                start = std::chrono::steady_clock::now();
            }
        }
    });

    while (request != "exit" && request != "peer"){
        printf("Нужна помощь - напишите 'help'\n");
        std::getline(std::cin, request); 

        if (request.empty())
            continue;

        if (request.starts_with("help")){
            printHelpForServer();
        }
        else{
            {
                const std::lock_guard<std::mutex> lock(mutex); //критическая секция для отправки
                socketClientUdp.send(address, {request.begin(), request.end()});
            }
            //получаем всегда сообщение от сервера и выводим на экран, или если это про порт, то выйдем и сразу сделаем то, что надо
            buf = socketClientUdp.receive(address);
            response = {buf.begin(), buf.end()};

            if (response.starts_with("in") || response.starts_with("out")){
                request = "peer";
            }
            else{
                std::cout << response << std::endl;
            }
        }
    }
    
    isStopAlive = true; //отключаем поток
    thr_keep_alive.join();

    if (request != "exit"){
        //условное для простоты проверки
        std::string where = response.substr(0, response.find(" "));
        uint16_t port_in1 = std::atoi(response.substr(response.find(" ") + 1).c_str());
        uint16_t port_in2 = port_in1 + 50;

        if (where == "in"){
            //отправляем в поток. то с чем мы будем взаимодействовать
            SocketAddress addressServer(ip, port_in1);
            SocketTcpServer socketTcpServer(addressServer); 
            std::thread thr1([&socketTcpServer](){
                try{
                    socketTcpServer.accept();
                    interface_receiver(socketTcpServer);

                    socketTcpServer.close();
                }catch(SocketException &e){}
            });

            //чем мы взаимодействуем
            SocketAddress addressPeer(ip, port_in2);
            SocketTCP socketClientTcp(addressPeer);

            interface_sender(socketClientTcp);

            socketClientTcp.close();
            thr1.join(); //ждем полного отсоединения
        }
        else{
            SocketAddress addressServer(ip, port_in2);
            SocketTcpServer socketTcpServer(addressServer); 
            std::thread thr2([&socketTcpServer](){
                try{
                    socketTcpServer.accept();
                    interface_receiver(socketTcpServer);

                    socketTcpServer.close();
                }catch(SocketException &e){}
            });

            //чем мы взаимодействуем
            SocketAddress addressPeer(ip, port_in1);
            SocketTCP socketClientTcp(addressPeer);

            interface_sender(socketClientTcp);

            socketClientTcp.close();
            thr2.join(); //ждем полного отсоединения
        }
        socketClientUdp.close();
    }
}
catch(SocketException &e){
    std::cout << e.what() << std::endl;
}
catch(std::exception &e){
    std::cout << e.what() << std::endl;
}


    return 0;
}