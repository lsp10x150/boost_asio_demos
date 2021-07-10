#include <iostream>
#include <boost/asio.hpp>

void synchronous_connection_demo() {
    using namespace boost::asio;
    io_context ioContext;
    ip::tcp::resolver resolver{ioContext};
    ip::tcp::socket socket{ioContext};
    try {
        auto endpoints = resolver.resolve("www.yandex.ru", "http");
        const auto connected_endpoint = connect(socket, endpoints);
        std::cout << connected_endpoint;
    } catch(const boost::system::system_error& se) {
        std::cerr << "Error: " << se.what() << '\n';
    }
}

void asynchronous_connection_demo() {
    using namespace boost::asio;
    io_context ioContext;
    ip::tcp::resolver resolver{ioContext};
    ip::tcp::socket socket{ioContext};
    try {
        auto endpoints = resolver.resolve("www.yandex.ru", "http");
        async_connect(socket, endpoints, [](boost::system::error_code ec, const auto& e){
            std::cout << e;
        });
    } catch(const boost::system::system_error& se) {
        std::cerr << "Error: " << se.what() << '\n';
    }
    ioContext.run();
}

int main() {
    asynchronous_connection_demo();
}
