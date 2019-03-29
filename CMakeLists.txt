cmake_minimum_required(VERSION 3.13)
project(server_proj)

set(CMAKE_CXX_STANDARD 11)


include_directories(/home/alexsmith/server_proj/cmake-build-debug/ /opt/libtcod/libtcod/include/ /home/alexsmith/sokoban_proj/cmake-build-debug/)
link_directories(/opt/libtcod/libtcod/ /home/alexsmith/sokoban_proj/cmake-build-debug/)

#cmake-build-debug/sec_client.cpp
#cmake-build-debug/cl_main2.cpp
add_executable(server main.cpp)
add_executable(client cmake-build-debug/cl_main.cpp cmake-build-debug/TwoDimArray.h)

target_link_libraries(server -ltcod -ltcodxx)
target_link_libraries(client -ltcod -ltcodxx)
#target_link_libraries(server -lboost_system -lboost_thread -lpthread)
#target_link_libraries(client -lboost_system -lboost_thread -lpthread)