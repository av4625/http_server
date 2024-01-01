#ifndef HTTP_SERVER_IMPL_HPP
#define HTTP_SERVER_IMPL_HPP

#include <memory>

#include <boost/asio.hpp>
#include <boost/beast/core/error.hpp>

#include <http/server.hpp>

#include "request_handler_fwd.hpp"
#include "session_manager_fwd.hpp"

namespace http
{

class server_impl :
    public server,
    public std::enable_shared_from_this<server_impl>
{
public:
    server_impl(
        const std::string& address,
        const std::string& port,
        const std::shared_ptr<boost::asio::io_context>& io_context,
        const std::shared_ptr<session_manager>& session_manager,
        const std::shared_ptr<request_handler>& request_handler);

    void start_server() override;

    void end_server() override;

    void serve_static(const std::string& path) override;

    void on(
        const std::string& uri,
        method method,
        std::function<void(const request&, file_response&)> callback) override;

    void on(
        const std::string& uri,
        method method,
        std::function<void(const request&, string_response&)> callback) override;

    void add_generic_request_handler(
        std::function<void(const request&, file_response&)> callback) override;

    void add_generic_request_handler(
        std::function<void(const request&, string_response&)> callback) override;

    void body_limit(std::uint64_t limit) override;

    void reset() override;

private:
    // Initiate an asynchronous accept operation.
    void start_accept();

    // Handle completion of an asynchronous accept operation.
    void handle_accept(
        const boost::beast::error_code& ec,
        boost::asio::ip::tcp::socket socket);

    // The io_context used to perform asynchronous operations.
    const std::shared_ptr<boost::asio::io_context> io_context_;

    // Acceptor used to listen for incoming connections.
    boost::asio::ip::tcp::acceptor acceptor_;

    const std::string address_;

    const std::string port_;

    const std::shared_ptr<session_manager> session_manager_;

    const std::shared_ptr<request_handler> request_handler_;

    std::uint64_t body_limit_;
};

}

#endif
