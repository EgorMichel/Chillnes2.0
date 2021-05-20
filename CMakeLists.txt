cmake_minimum_required(VERSION 3.17.5)
project(Chillnes2_0)

set(CMAKE_CXX_STANDARD 20)

find_package(SFML 2.5 COMPONENTS graphics audio network REQUIRED)

add_executable(Chillnes2_0 latest.cpp)

target_link_libraries(Chillnes2_0 sfml-graphics sfml-audio sfml-network)
