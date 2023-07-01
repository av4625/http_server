#include "server_impl.hpp"

#include <boost/beast/core/bind_handler.hpp>

#include "session_impl.hpp"
#include "request_handler.hpp"
#include "session_manager.hpp"

namespace http
{

server_impl::server_impl(
    const std::string& address,
    const std::string& port,
    const std::shared_ptr<boost::asio::io_context>& io_context,
    const std::shared_ptr<session_manager>& session_manager,
    const std::shared_ptr<request_handler>& request_handler) :
        io_context_(io_context),
        // Accept in a strand, can't accept concurrently with this strand
        acceptor_(boost::asio::make_strand(*io_context_)),
        address_(address),
        port_(port),
        session_manager_(std::move(session_manager)),
        request_handler_(request_handler),
        body_limit_(1048576)
{
}

void server_impl::start_server()
{
    // Open the acceptor with the option to reuse the address (i.e. SO_REUSEADDR).
    // To-Do: These can throw
    boost::asio::ip::tcp::resolver resolver(*io_context_);
    boost::asio::ip::tcp::endpoint endpoint =
        *resolver.resolve(address_, port_).begin();
    acceptor_.open(endpoint.protocol());
    acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
    acceptor_.bind(endpoint);
    acceptor_.listen(boost::asio::ip::tcp::acceptor::max_listen_connections);
    start_accept();
}

void server_impl::end_server()
{
    /* The server is stopped by cancelling all outstanding asynchronous
       operations */
    acceptor_.close();
    session_manager_->stop_all();
}

void server_impl::serve_static(const std::string& path)
{
    request_handler_->serve_from_directory(path);
}

void server_impl::on(
    const std::string& uri,
    const method method,
    std::function<void(const request&, file_response&)> callback)
{
    request_handler_->add_request_handler(uri, method, std::move(callback));
}

void server_impl::on(
    const std::string& uri,
    const method method,
    std::function<void(const request&, string_response&)> callback)
{
    request_handler_->add_request_handler(uri, method, std::move(callback));
}

void server_impl::body_limit(const std::uint64_t limit)
{
    body_limit_ = limit;
}

void server_impl::reset()
{
    request_handler_->reset();
}

void server_impl::start_accept()
{
    // Provide a new strand per connection
    acceptor_.async_accept(
        boost::asio::make_strand(*io_context_),
        boost::beast::bind_front_handler(
            &server_impl::handle_accept,
            shared_from_this()));
}

void server_impl::handle_accept(
    const boost::beast::error_code& ec,
    boost::asio::ip::tcp::socket socket)
{
    /* Check whether the server was stopped before this completion handler had a
       chance to run. */
    if (!acceptor_.is_open())
    {
        return;
    }

    if(ec)
    {
        // To avoid infinite loop
        return;
    }
    else
    {
        /* Create the session and run it
           Because the acceptor uses a strand, this function will never be run
           on different threads, this means that the session manager doesn't
           have to be thread safe */
        session_manager_->start(
            std::make_shared<session_impl>(
                std::move(socket),
                request_handler_,
                session_manager_,
                body_limit_));
    }

    start_accept();
}

}
