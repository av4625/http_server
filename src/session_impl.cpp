#include "session_impl.hpp"

#include <boost/asio/dispatch.hpp>
#include <boost/beast/core/bind_handler.hpp>
#include <boost/beast/core/buffers_generator.hpp>
#include <boost/beast/http/read.hpp>

#include "request_handler.hpp"
#include "session_manager.hpp"

namespace http
{

session_impl::session_impl(
    boost::asio::ip::tcp::socket&& socket,
    const std::shared_ptr<request_handler>& request_handler,
    const std::shared_ptr<session_manager>& session_manager,
    const std::uint64_t limit) :
        stream_(std::move(socket)),
        request_handler_(request_handler),
        session_manager_(session_manager),
        buffer_(),
        parser_(),
        body_limit_(limit)
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
    // Construct a new parser for each message
    parser_.emplace();
    parser_->body_limit(body_limit_);

    // Set the timeout.
    stream_.expires_after(std::chrono::seconds(30));

    // Read a request
    boost::beast::http::async_read(
        stream_,
        buffer_,
        *parser_,
        boost::beast::bind_front_handler(
            &session_impl::on_read,
            shared_from_this()));
}

void session_impl::on_read(
    const boost::beast::error_code& ec, const std::size_t)
{
    // boost::beast::http::error::end_of_stream means they closed the connection
    if(ec)
    {
        if (ec == boost::beast::http::error::body_limit)
        {
            send_response(
                request_handler_->body_limit_reached(parser_->release()));
        }
        else if (ec != boost::asio::error::operation_aborted)
        {
            session_manager_->stop(shared_from_this());
        }

        return;
    }

    // Send the response
    send_response(request_handler_->handle_request(parser_->release()));
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
        if (ec != boost::asio::error::operation_aborted)
        {
            session_manager_->stop(shared_from_this());
        }

        return;
    }

    if(!keep_alive)
    {
        /* This means we should close the connection, usually because the
           response indicated the "Connection: close" semantic. */
        session_manager_->stop(shared_from_this());
        return;
    }

    // Read another request
    do_read();
}

void session_impl::do_close()
{
    // Send a TCP shutdown
    boost::beast::error_code ec;
    stream_.socket().shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
    /* Should cancel be used? If so handlers would need to call session manager
       stop when operation aborted or they will never be removed from the
       manager */
    // stream_.cancel();

    /* At this point the connection is closed gracefully, any outstanding read
       or write handlers will be called with an end of stream error code */
}

}
