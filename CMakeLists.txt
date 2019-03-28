cmake_minimum_required(VERSION 3.13)
project(server_proj)

set(CMAKE_CXX_STANDARD 11)


include_directories(/home/akuznetsov/server_proj/cmake-build-debug/)
#link_directories(/home/akuznetsov/boost/ /home/akuznetsov/boost/boost/asio/ /home/akuznetsov/server_proj/cmake-build-debug/boost/boost/asio/async_result.hpp)

#cmake-build-debug/sec_client.cpp
#cmake-build-debug/cl_main2.cpp
add_executable(server main.cpp)
add_executable(client cmake-build-debug/cl_main.cpp)
add_executable(client2 cmake-build-debug/cl_main2.cpp)
#target_link_libraries(server -lboost_system -lboost_thread -lpthread)
#target_link_libraries(client -lboost_system -lboost_thread -lpthread)