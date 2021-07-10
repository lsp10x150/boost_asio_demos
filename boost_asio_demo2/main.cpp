#include <iostream>
#include <boost/asio.hpp>

void synchronous_resolver_demo() {
    boost::asio::io_context io_context{8};
    boost::asio::ip::tcp::resolver my_resolver{io_context};
    boost::system::error_code ec;
    for (auto&& result : my_resolver.resolve("www.yandex.ru", "http", ec)) {
        std::cout << result.service_name() << ' '
                  << result.host_name() << ' '
                  << result.endpoint() <<  '\n';
    }
    if (ec) std::cout << "Error code: " << ec << std::endl;
}

void asynchronous_resolver_demo() {
    boost::asio::io_context io_context{8};
    boost::asio::ip::tcp::resolver my_resolver{io_context};
    boost::system::error_code ec;
    my_resolver.async_resolve("www.yandex.ru", "http",
                              [](boost::system::error_code ec, const auto& results){
        if (ec) {
            std::cerr << "Error: " << ec << '\n';
            return;
        }
        for (auto&& result : results) {
        std::cout << result.service_name() << ' '
                  << result.host_name() << ' '
                  << result.endpoint() <<  '\n';
        }
    });
    io_context.run(); //! It is very important to run io_context, because we must wait for async operations
}

int main() {
    asynchronous_resolver_demo();
}
