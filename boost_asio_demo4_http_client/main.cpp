#include <iostream>
#include <boost/asio.hpp>
#include <string>

std::string request(std::string_view host, boost::asio::io_context& io_context) {
    std::stringstream request_stream;
    request_stream << "GET / HTTP/1.1\r\n"
                   << "Host: " << host << "\r\n"
                   << "Accept: text/html\r\n"
                   << "Accept-Language: en-us\r\n"
                   << "Accept-Encoding: identity\r\n"
                   << "Connection: close\r\n\r\n";
    boost::asio::ip::tcp::resolver resolver{io_context};
    const auto endpoints = resolver.resolve(host, "http");
    boost::asio::ip::tcp::socket socket{io_context};
    const auto connected_endpoint = boost::asio::connect(socket, endpoints);
    boost::asio::write(socket, boost::asio::buffer(request_stream.str()));
    std::string response;
    boost::system::error_code ec;
    boost::asio::read(socket, boost::asio::dynamic_buffer(response), ec);
    if (ec && ec.value() != 2) throw boost::system::system_error(ec);
    return response;
}

namespace async {
    using ResolverResult = boost::asio::ip::tcp::resolver::results_type;
    using Endpoint = boost::asio::ip::tcp::endpoint;
    struct Request {
        explicit Request(boost::asio::io_context& io_context, std::string host)
            : resolver(io_context)
            , socket(io_context)
            , host(std::move(host))
        {
            std::stringstream request_stream;
            request_stream << "GET / HTTP/1.1\r\n"
                           << "Host: " << host << "\r\n"
                           << "Accept: text/html\r\n"
                           << "Accept-Language: en-us\r\n"
                           << "Accept-Encoding: identity\r\n"
                           << "Connection: close\r\n\r\n";
            request = request_stream.str();
            resolver.async_resolve(this->host, "http",
                                   [this](boost::system::error_code ec, const ResolverResult& results) {
                resolution_handler(ec, results);
            });
        }

        void resolution_handler(boost::system::error_code ec, const ResolverResult& results) {
            if (ec) {
                std::cerr << "Error resolving " << host << ": " << ec << '\n';
                return;
            }
            boost::asio::async_connect(socket, results,
                                       [this](boost::system::error_code ec, const Endpoint& endpoint) {
                connection_handler(ec, endpoint);
            });
        }

        void connection_handler(boost::system::error_code ec, const Endpoint& endpoint) {
            if (ec) {
                std::cerr << "Error connecting to " << host << ": " << ec.message() << '\n';
                return;
            }
            boost::asio::async_write(socket, boost::asio::buffer(request),
                                     [this](boost::system::error_code ec, size_t transferred) {
                write_handler(ec, transferred);
            });
        }

        void write_handler(boost::system::error_code ec, size_t transferred) {
            if (ec) {
                std::cerr << "Error writing to " << host << ": " << ec.message() << '\n';
            } else if (request.size() != transferred) {
                request.erase(0, transferred);
                boost::asio::async_write(socket, boost::asio::buffer(request),
                                         [this](boost::system::error_code ec,
                                                 size_t transferred) {
                    write_handler(ec, transferred);
                });
            } else {
                boost::asio::async_read(socket, boost::asio::dynamic_buffer(response),
                                        [this](boost::system::error_code ec,
                                                size_t transferred) {
                    read_handler(ec, transferred);
                });
            }
        }

        void read_handler(boost::system::error_code ec, size_t transferred) {
            if (ec && ec.value() != 2)
                std::cerr << "Error reading from " << host << ": " << ec.message() << '\n';
        }

        [[nodiscard]] const std::string& get_response() const noexcept {
            return response;
        }

    private:
        boost::asio::ip::tcp::resolver resolver;
        boost::asio::ip::tcp::socket socket;
        std::string request, response;
        std::string host;
    };
}

void sync_request(int argc, char** argv) {
    if (argc != 2) throw std::runtime_error("It is imperative to provide argument");
    boost::asio::io_context io_context;
    try {
        const std::string_view host = argv[1];
        const auto response = request(host, io_context);
        std::cout << response << '\n';
    } catch (const boost::system::system_error& se) {
        std::cerr << "Error: " << se.what() << '\n';
    }
    std::cout.flush();
}

void async_request(int argc, char** argv) {
    boost::asio::io_context ioContext;
    async::Request request(ioContext, argv[1]);
    ioContext.run();
    std::cout << request.get_response() << '\n';
}

int main(int argc, char** argv) {
    async_request(argc, argv);
}
