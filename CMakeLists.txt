#1.cmake verson，指定cmake版本
cmake_minimum_required(VERSION 3.1)

#2.project name，指定项目的名称，一般和项目的文件夹名称对应
PROJECT(libevent_test)

#设置编译器版本
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

#client_cj
file(GLOB_RECURSE CLIENT_CJ_SOURCES cj/client.cpp)
add_executable(client_cj ${CLIENT_CJ_SOURCES})
target_link_libraries(client_cj event)

#server_cj
file(GLOB_RECURSE SERVER_CJ_SOURCES cj/server.cpp)
add_executable(server_cj ${SERVER_CJ_SOURCES})
target_link_libraries(server_cj event)

#client_zj
file(GLOB_RECURSE CLIENT_ZJ_SOURCES zj/client.cpp)
add_executable(client_zj ${CLIENT_ZJ_SOURCES})
target_link_libraries(client_zj event)

#server_zj
file(GLOB_RECURSE SERVER_ZJ_SOURCES zj/server.cpp)
add_executable(server_zj ${SERVER_ZJ_SOURCES})
target_link_libraries(server_zj event)

#client_gj
file(GLOB_RECURSE CLIENT_GJ_SOURCES gj/client.cpp)
add_executable(client_gj ${CLIENT_GJ_SOURCES})
target_link_libraries(client_gj event)

#server_gj
file(GLOB_RECURSE SERVER_GJ_SOURCES gj/server.cpp)
add_executable(server_gj ${SERVER_GJ_SOURCES})
target_link_libraries(server_gj event)
