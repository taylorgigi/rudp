cmake_minimum_required(VERSION 3.11.2)
project(rudp_core VERSION 1.0.0.0 LANGUAGES CXX)

SET(CMAKE_CXX_STANDARD 17)

include_directories(AFTER SYSTEM /usr/local/include $ENV{HOME}/boost_1_73_0 $ENV{TBBROOT}/include)
link_directories(/usr/local/lib $ENV{HOME}/boost_1_73_0/stage/lib $ENV{TBBROOT}/build/linux_intel64_gcc_cc10.1.1_libc2.31_kernel5.7.6_release)

add_library(rudp_core SHARED UdpConnectionPool.cpp UdpConnection.cpp logger.cpp)
target_link_libraries(rudp_core boost_timer boost_thread boost_system boost_chrono pthread tcmalloc)

add_subdirectory(test)
