cmake_minimum_required(VERSION 3.19)
project(boost_asio_demo5_server)

set(CMAKE_CXX_STANDARD 20)
find_package(Boost REQUIRED system)
add_executable(boost_asio_demo5_server main.cpp)
target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES} pthread)