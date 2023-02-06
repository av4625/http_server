#ifndef HTTP_SESSION_IMPL_HPP
#define HTTP_SESSION_IMPL_HPP

#include <memory>

#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/core/flat_buffer.hpp>
#include <boost/beast/core/tcp_stream.hpp>
#include <boost/beast/http/message.hpp>
#include <boost/beast/http/message_generator.hpp>
#include <boost/beast/http/string_body.hpp>

#include "request_handler_fwd.hpp"
#include "session.hpp"

namespace http
{

class session_impl :
    public session, public std::enable_shared_from_this<session_impl>
{
public:
    session_impl(
        boost::asio::ip::tcp::socket&& socket,
        const std::shared_ptr<request_handler>& request_handler);

    void run() override;

    void stop() override;

private:
    boost::beast::tcp_stream stream_;
    const std::shared_ptr<request_handler>& request_handler_;
    boost::beast::flat_buffer buffer_;
    boost::beast::http::request<boost::beast::http::string_body> request_;

    void do_read();

    void on_read(
        const boost::beast::error_code& ec, std::size_t bytes_transferred);

    void send_response(boost::beast::http::message_generator&& msg);

    void on_write(
        bool keep_alive,
        const boost::beast::error_code& ec,
        std::size_t bytes_transferred);

    void do_close();
};

}

#endif
