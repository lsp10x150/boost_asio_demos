cmake_minimum_required(VERSION 3.19)
project(Boost_asio)
set(CMAKE_CXX_STANDARD 20)

find_package(Boost 1.40.0 REQUIRED system)

add_executable(Boost_asio main.cpp)
target_link_libraries(Boost_asio ${Boost_LIBRARIES} pthread)
