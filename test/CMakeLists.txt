cmake_minimum_required(VERSION 3.0)

set(CMAKE_CXX_STANDARD 17)
project(test)

set(SOURCE_SOCKET_ADDRESS 
    ../socketAddress/socketAddress.h
    ../socketAddress/socketAddress.cpp)

set(SOURCE_DIRECTORY
    ../directory/directory.h
    ../directory/directory.cpp)

set(SOURCE_MESSAGE_PACK
../messagePack/messagePack.h
../messagePack/messagePack.cpp)

set(SOURCE_MESSAGE
../message/message.h
../message/message.cpp)

set(SOURCE_MESSAGE_UNPACK 
../messageUnPack/messageUnPack.h
../messageUnPack/messageUnPack.cpp)

find_package(GTest REQUIRED)
add_executable(${PROJECT_NAME} 
${SOURCE_SOCKET_ADDRESS}
${SOURCE_DIRECTORY}
${SOURCE_MESSAGE}
${SOURCE_MESSAGE_PACK}
${SOURCE_MESSAGE_UNPACK}
test.cpp
)
target_link_libraries(${PROJECT_NAME} ${GTEST_LIBRARIES})
