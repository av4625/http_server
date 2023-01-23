#include "connection_impl.hpp"

#include <vector>

#include <boost/bind/bind.hpp>

#include "connection_manager.hpp"
#include "request_handler.hpp"
#include "status_code.hpp"
#include "stock_responses.hpp"

namespace http
{

connection_impl::connection_impl(
    const std::shared_ptr<boost::asio::io_context>& io_context,
    const std::shared_ptr<connection_manager>& manager,
    const std::shared_ptr<request_handler>& handler) :
        socket_(*io_context),
        connection_manager_(manager),
        request_handler_(handler),
        buffer_(),
        request_(),
        request_parser_(),
        response_()
{
}

boost::asio::ip::tcp::socket& connection_impl::socket()
{
    return socket_;
}

void connection_impl::start()
{
    socket_.async_read_some(
        boost::asio::buffer(buffer_),
        boost::bind(
            &connection_impl::handle_read,
            shared_from_this(),
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));
}

void connection_impl::stop()
{
    socket_.close();
}

void connection_impl::handle_read(
    const boost::system::error_code& e,
    const std::size_t bytes_transferred)
{
    if (!e)
    {
        boost::tribool result;
        boost::tie(result, boost::tuples::ignore) = request_parser_.parse(
            request_, buffer_.data(), buffer_.data() + bytes_transferred);

        if (result)
        {
            request_handler_->handle_request(request_, response_);
            boost::asio::async_write(
                socket_,
                response_.to_buffers(),
                boost::bind(
                    &connection_impl::handle_write,
                    shared_from_this(),
                    boost::asio::placeholders::error));
        }
        else if (result == boost::indeterminate)
        {
            socket_.async_read_some(
                boost::asio::buffer(buffer_),
                boost::bind(
                    &connection_impl::handle_read,
                    shared_from_this(),
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));
        }
        else
        {
            response_ = stock_reply(status_code::bad_request);
            boost::asio::async_write(
                socket_,
                response_.to_buffers(),
                boost::bind(
                    &connection_impl::handle_write,
                    shared_from_this(),
                    boost::asio::placeholders::error));
        }
    }
    else if (e != boost::asio::error::operation_aborted)
    {
        connection_manager_->stop(shared_from_this());
    }
}

void connection_impl::handle_write(const boost::system::error_code& e)
{
    if (!e)
    {
        // Initiate graceful connection closure.
        boost::system::error_code ignored_ec;
        socket_.shutdown(
            boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);
    }

    if (e != boost::asio::error::operation_aborted)
    {
        connection_manager_->stop(shared_from_this());
    }
}

}
