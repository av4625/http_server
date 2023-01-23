#ifndef HTTP_CONNECTION_IMPL_HPP
#define HTTP_CONNECTION_IMPL_HPP

#include <memory>

#include <boost/array.hpp>

#include "connection.hpp"
#include "connection_manager_fwd.hpp"
#include "response.hpp"
#include "request_data.hpp"
#include "request_handler_fwd.hpp"
#include "request_parser.hpp"

namespace http
{

class connection_impl :
    public std::enable_shared_from_this<connection_impl>,
    public connection
{
public:
    explicit connection_impl(
        const std::shared_ptr<boost::asio::io_context>& io_context,
        const std::shared_ptr<connection_manager>& manager,
        const std::shared_ptr<request_handler>& handler);

    boost::asio::ip::tcp::socket& socket() override;

    void start() override;

    void stop() override;

private:
    // Handle completion of a read operation.
    void handle_read(
        const boost::system::error_code& e,
        std::size_t bytes_transferred);

    // Handle completion of a write operation.
    void handle_write(const boost::system::error_code& e);

    // Socket for the connection.
    boost::asio::ip::tcp::socket socket_;

    // The manager for this connection.
    const std::shared_ptr<connection_manager> connection_manager_;

    // The handler used to process the incoming request.
    const std::shared_ptr<request_handler> request_handler_;

    // Buffer for incoming data.
    boost::array<char, 8192> buffer_;

    // The incoming request.
    request_data request_;

    // The parser for the incoming request.
    request_parser request_parser_;

    // The response to be sent back to the client.
    response response_;
};

}

#endif
