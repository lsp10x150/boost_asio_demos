#include <iostream>
#include <boost/asio.hpp>
#include <boost/algorithm/string/case_conv.hpp>

using namespace boost::asio;

void handle(ip::tcp::socket& socket) {
    boost::system::error_code ec;
    std::string message;
    do {
        boost::asio::read_until(socket, dynamic_buffer(message), "\n");
        boost::algorithm::to_upper(message);
        boost::asio::write(socket, buffer(message), ec);
        if (message == "\n") return;
        message.clear();
    } while(!ec);
}

#include <memory>

struct Session : std::enable_shared_from_this<Session> {
    explicit Session(boost::asio::ip::tcp::socket socket)
        : socket(std::move(socket))
    {}

    void read() {
        boost::asio::async_read_until(socket, dynamic_buffer(message), '\n',
                                      [self=shared_from_this()] (boost::system::error_code ec, std::size_t length)
                                      {if (ec || self->message == "\n") return;
                                      boost::algorithm::to_upper(self->message);
                                      self->write();
                                      });
    }

    void write() {
        async_write(socket, boost::asio::buffer(message),
                    [self=shared_from_this()](boost::system::error_code ec, std::size_t length){
            if (ec) return;
            self->message.clear();
            self->read();
        });
    }

private:
    boost::asio::ip::tcp::socket socket;
    std::string message;
};

void sync_server() {
    try {
        io_context io_context;
        ip::tcp::acceptor acceptor(io_context, ip::tcp::endpoint(ip::tcp::v4(), 1895));
        while (true) {
            ip::tcp::socket socket(io_context);
            acceptor.accept(socket);
            handle(socket);
        }
    } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
    }
}

void serve(boost::asio::ip::tcp::acceptor& acceptor) {
    acceptor.async_accept([&acceptor](boost::system::error_code ec, boost::asio::ip::tcp::socket socket) {
        serve(acceptor);
        if (ec) return;
        auto session = std::make_shared<Session>(std::move(socket));
        session->read();
    });
}

void async_server() {
    try {
        boost::asio::io_context io_context;
        boost::asio::ip::tcp::acceptor acceptor(io_context, boost::asio::ip::tcp::endpoint(
                boost::asio::ip::tcp::v4(), 1895));
        serve(acceptor);
        io_context.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
    }
}

int main() {
    async_server();
}
