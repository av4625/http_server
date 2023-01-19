#include "server_impl.hpp"

#include <signal.h>

#include <boost/bind/bind.hpp>

#include "connection_impl.hpp"
#include "connection_manager.hpp"
#include "request_handler.hpp"

namespace http
{

server_impl::server_impl(
    const std::string& address,
    const std::string& port,
    const std::shared_ptr<boost::asio::io_context>& io_context,
    const std::shared_ptr<connection_manager>& connection_manager,
    const std::shared_ptr<request_handler>& request_handler) :
        io_context_(io_context),
        acceptor_(*io_context_),
        connection_manager_(connection_manager),
        new_connection_(),
        request_handler_(request_handler),
        address_(address),
        port_(port)
{
}

void server_impl::start_server()
{
    // Open the acceptor with the option to reuse the address (i.e. SO_REUSEADDR).
    boost::asio::ip::tcp::resolver resolver(*io_context_);
    boost::asio::ip::tcp::endpoint endpoint =
        *resolver.resolve(address_, port_).begin();
    acceptor_.open(endpoint.protocol());
    acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
    acceptor_.bind(endpoint);
    acceptor_.listen();
    start_accept();
}

void server_impl::end_server()
{
    /* The server is stopped by cancelling all outstanding asynchronous
       operations */
    acceptor_.close();
    connection_manager_->stop_all();
}

void server_impl::serve_static(const std::string& path)
{
    request_handler_->serve_from_directory(path);
}

void server_impl::on(
    const std::string& uri,
    const method method,
    std::function<void(const request&, response&)> callback)
{
    request_handler_->add_request_handler(uri, method, std::move(callback));
}

void server_impl::start_accept()
{
    new_connection_.reset(new connection_impl(
        io_context_,
        connection_manager_,
        request_handler_));
    acceptor_.async_accept(
        new_connection_->socket(),
        boost::bind(
            &server_impl::handle_accept,
            // To-Do: shared_from_this?
            this,
            boost::asio::placeholders::error));
}

void server_impl::handle_accept(const boost::system::error_code& e)
{
    /* Check whether the server was stopped before this completion handler had a
       chance to run. */
    if (!acceptor_.is_open())
    {
        return;
    }

    if (!e)
    {
        connection_manager_->start(new_connection_);
    }

    start_accept();
}

}
