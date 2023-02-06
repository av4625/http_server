#include "session_impl.hpp"

#include <boost/asio/dispatch.hpp>
#include <boost/beast/core/bind_handler.hpp>
#include <boost/beast/core/buffers_generator.hpp>
#include <boost/beast/http/read.hpp>

#include "request_handler.hpp"

namespace http
{

session_impl::session_impl(
    boost::asio::ip::tcp::socket&& socket,
    const std::shared_ptr<request_handler>& request_handler) :
    stream_(std::move(socket)),
    request_handler_(request_handler),
    buffer_(),
    request_()
{
}

void session_impl::run()
{
    /* Need to execute all session operations within a strand so that multiple
       sessions can run concurrently */
    boost::asio::dispatch(
        stream_.get_executor(),
        boost::beast::bind_front_handler(
            &session_impl::do_read,
            shared_from_this()));
}

void session_impl::stop()
{
    do_close();
}

void session_impl::do_read()
{
    /* Make the request empty before reading, otherwise the operation behavior
       is undefined as it was moved out */
    request_ = {};

    // Set the timeout.
    stream_.expires_after(std::chrono::seconds(30));

    // Read a request
    boost::beast::http::async_read(
        stream_,
        buffer_,
        request_,
        boost::beast::bind_front_handler(
            &session_impl::on_read,
            shared_from_this()));
}

void session_impl::on_read(
    const boost::beast::error_code& ec, const std::size_t)
{
    // This means they closed the connection
    if(ec == boost::beast::http::error::end_of_stream)
    {
        return do_close();
    }

    if(ec)
    {
        // Log
        return;
    }

    // Send the response
    send_response(request_handler_->handle_request(std::move(request_)));
}

void session_impl::send_response(boost::beast::http::message_generator&& msg)
{
    const bool keep_alive{msg.keep_alive()};
    // Write the response
    boost::beast::async_write(
        stream_,
        std::move(msg),
        boost::beast::bind_front_handler(
            &session_impl::on_write, shared_from_this(), keep_alive));
}

void session_impl::on_write(
    const bool keep_alive,
    const boost::beast::error_code& ec,
    const std::size_t)
{
    if(ec)
    {
        // Log
        return;
    }

    if(!keep_alive)
    {
        /* This means we should close the connection, usually because the
           response indicated the "Connection: close" semantic. */
        return do_close();
    }

    // Read another request
    do_read();
}

void session_impl::do_close()
{
    // Send a TCP shutdown
    boost::beast::error_code ec;
    stream_.socket().shutdown(boost::asio::ip::tcp::socket::shutdown_send, ec);

    // At this point the connection is closed gracefully
}

}
