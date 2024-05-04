cmake_minimum_required(VERSION 3.0) #Проверка версии CMake#если установлена меньше, то ошибка#проверка версии cmake --version

set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall  -Wno-unused-variable -Wno-sign-compare -Wno-missing-braces -Wno-unused-function -std=c++20")

project(client)                #Название проекта

set(SOURCE_EXE client.cpp)            #Установка переменной со списком исходников(через пробел) для мсполняемого файлв

set(SOURCE_SOCKET_ADDRESS 
    ../socketAddress/socketAddress.h 
    ../socketAddress/socketAddress.cpp)

set(SOURCE_SOCKET_EXCEPTION 
    ../socketException/socketException.h 
    ../socketException/socketException.cpp)

set(SOURCE_SOCKET 
    ../socket/socket.h 
    ../socket/socket.cpp)

set(SOURCE_SOCKET_UDP 
    ../socketUdp/socketUDP.h 
    ../socketUdp/socketUDP.cpp)

set(SOURCE_SOCKET_TCP
    ../socketTcp/socketTCP.h 
    ../socketTcp/socketTCP.cpp)

set(SOURCE_SOCKET_TCP_SERVER
    ../socketTcpServer/socketTcpServer.h
    ../socketTcpServer/socketTcpServer.cpp)

set(SOURCE_MESSAGE
    ../message/message.h
    ../message/message.cpp)

set(SOURCE_MESSAGE_PACK
    ../messagePack/messagePack.h
    ../messagePack/messagePack.cpp)  

set(SOURCE_MESSAGE_UNPACK
    ../messageUnPack/messageUnPack.h
    ../messageUnPAck/messageUnPack.cpp)

set(SOURCE_DIRECTORY
    ../directory/directory.h
    ../directory/directory.cpp)

set(SOURCE_TRANSFER_FILES
    ../transferFiles/transferFiles.h
    ../transferFiles/transferFiles.cpp)

set(SOURCE_INTERFACE
    ../interface/interface.h
    ../interface/interface.cpp)

add_executable(${PROJECT_NAME}
                    ${SOURCE_SOCKET_ADDRESS}
                    ${SOURCE_SOCKET_EXCEPTION}
                    ${SOURCE_SOCKET}
                    ${SOURCE_SOCKET_UDP}
                    ${SOURCE_SOCKET_TCP}
                    ${SOURCE_SOCKET_TCP_SERVER}
                    ${SOURCE_MESSAGE}
                    ${SOURCE_MESSAGE_PACK}
                    ${SOURCE_MESSAGE_UNPACK}
                    ${SOURCE_DIRECTORY}
                    ${SOURCE_TRANSFER_FILES}
                    ${SOURCE_INTERFACE}
                    ${SOURCE_EXE}
)
