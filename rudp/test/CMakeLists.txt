cmake_minimum_required(VERSION 3.11.2)
project(test VERSION 1.0.0.0 LANGUAGES CXX)

SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_COMPILER g++)
SET(CMAKE_CXX_FLAGS "-g -Wall -O0")

add_executable(client client.cpp)
target_link_libraries(client rudp_core yaml-cpp tbb boost_timer boost_thread boost_system boost_chrono boost_filesystem pthread tcmalloc)

add_executable(server server.cpp)
target_link_libraries(server rudp_core yaml-cpp tbb boost_timer boost_thread boost_system boost_chrono boost_filesystem pthread tcmalloc)

add_executable(UdpEndpointHasher-test UdpEndpointHasher-test.cpp)
target_link_libraries(UdpEndpointHasher-test boost_system pthread)
