cmake_minimum_required(VERSION 3.11.2)
project(proxy VERSION 1.0.0.0 LANGUAGES CXX)

SET(CMAKE_CXX_COMPILER "g++")
SET(CMAKE_CXX_FLAGS "-g -Wall -O0")
SET(CMAKE_CXX_STANDARD 11)

include_directories(AFTER SYSTEM /usr/local/include $ENV{HOME}/boost_1_73_0 $ENV{TBBROOT}/include)
link_directories(/usr/local/lib $ENV{HOME}/boost_1_73_0/stage/lib $ENV{TBBROOT}/build/linux_intel64_gcc_cc10.1.1_libc2.31_kernel5.7.6_release)

add_executable(client client.cpp logger.cpp UdpConnectionPool.cpp UdpConnection.cpp)
add_executable(server server.cpp logger.cpp UdpConnectionPool.cpp UdpConnection.cpp)
add_executable(UdpEndpointHasher-test UdpEndpointHasher-test.cpp)

target_link_libraries(client yaml-cpp tbb boost_timer boost_thread boost_system boost_chrono boost_filesystem pthread tcmalloc)
target_link_libraries(server yaml-cpp tbb boost_timer boost_thread boost_system boost_chrono boost_filesystem pthread tcmalloc)
target_link_libraries(UdpEndpointHasher-test boost_system pthread)

