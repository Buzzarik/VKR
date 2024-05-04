cmake_minimum_required(VERSION 3.0) #Проверка версии CMake
                                    #если установлена меньше, то ошибка
                                    #проверка версии cmake --version

set(CMAKE_CXX_FLAGS
    "${CMAKE_CXX_FLAGS} -Wall  -Wno-unused-variable -Wno-sign-compare -Wno-missing-braces -Wno-unused-function -std=c++20")

project(server)                #Название проекта

set(SOURCE_EXE server.cpp)            #Установка переменной со списком исходников(через пробел) для мсполняемого файлв

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

set(SOURCE_SOCKET_UDP_SERVER 
    ../socketUdpServer/socketUDPServer.h 
    ../socketUdpServer/socketUDPServer.cpp)

add_executable(${PROJECT_NAME}
                    ${SOURCE_SOCKET_ADDRESS}
                    ${SOURCE_SOCKET_EXCEPTION}
                    ${SOURCE_SOCKET}
                    ${SOURCE_SOCKET_UDP}
                    ${SOURCE_SOCKET_UDP_SERVER}
                    ${SOURCE_EXE}
)

# add_subdirectory(../socketAddress)               #Добавление подпроекта, указывается имя директории (сам проект создается отдельно)
# add_subdirectory(../socketException)               #Добавление подпроекта, указывается имя директории (сам проект создается отдельно)
# add_subdirectory(../socket)               #Добавление подпроекта, указывается имя директории (сам проект создается отдельно)     
# add_subdirectory(../socketUDP)               #Добавление подпроекта, указывается имя директории (сам проект создается отдельно)                                  
# add_subdirectory(../socketUDPServer)               #Добавление подпроекта, указывается имя директории (сам проект создается отдельно)

#target_link_libraries(main library_socket_udp_server)     #Линковка программы с библиотеками (через пробел)

#target_include_directories(main PUBLIC {SOURCE_DIR})