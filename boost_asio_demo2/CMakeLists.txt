cmake_minimum_required(VERSION 3.19)
project(boost_asio_demo2)

set(CMAKE_CXX_STANDARD 20)
find_package(Boost  REQUIRED system)
add_executable(boost_asio_demo2 main.cpp)
target_link_libraries(boost_asio_demo2 ${Boost_LIBRARIES} pthread)