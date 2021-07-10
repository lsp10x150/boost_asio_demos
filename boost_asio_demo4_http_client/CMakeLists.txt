cmake_minimum_required(VERSION 3.19)
project(boost_asio_demo4_http_client)

set(CMAKE_CXX_STANDARD 20)
find_package(Boost REQUIRED system)

add_executable(boost_asio_demo4_http_client main.cpp)
target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES} pthread)