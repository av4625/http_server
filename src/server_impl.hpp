#ifndef HTTP_SERVER_IMPL_HPP
#define HTTP_SERVER_IMPL_HPP

#include <memory>
#include <string>

#include <boost/asio.hpp>

#include "connection_fwd.hpp"
#include "connection_manager_fwd.hpp"
#include "request_handler_fwd.hpp"
#include "server.hpp"

namespace http
{

class server_impl : public server
{
public:
    server_impl(
        const std::string& address,
        const std::string& port,
        const std::shared_ptr<boost::asio::io_context>& io_context,
        const std::shared_ptr<connection_manager>& connection_manager,
        const std::shared_ptr<request_handler>& request_handler);

    void start_server() override;

    void end_server() override;

    void serve_static(const std::string& path) override;

    void on(
        const std::string& uri,
        method method,
        std::function<void(const request&, response&)> callback) override;

private:
    // Initiate an asynchronous accept operation.
    void start_accept();

    // Handle completion of an asynchronous accept operation.
    void handle_accept(const boost::system::error_code& e);

    // The io_context used to perform asynchronous operations.
    const std::shared_ptr<boost::asio::io_context> io_context_;

    // Acceptor used to listen for incoming connections.
    boost::asio::ip::tcp::acceptor acceptor_;

    // The connection manager which owns all live connections.
    const std::shared_ptr<connection_manager> connection_manager_;

    // The next connection to be accepted.
    std::shared_ptr<connection> new_connection_;

    // The handler for all incoming requests.
    const std::shared_ptr<request_handler> request_handler_;

    const std::string address_;

    const std::string port_;
};

}

#endif
