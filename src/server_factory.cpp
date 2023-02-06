#include <boost/asio.hpp>

#include <http/server_factory.hpp>

#include "request_handler_impl.hpp"
#include "server_impl.hpp"
#include "session_manager_impl.hpp"

namespace http
{

std::shared_ptr<server> server_factory::create(
    const std::string& port,
    const std::shared_ptr<boost::asio::io_context>& io_context) const
{
    return std::make_shared<server_impl>(
        "0.0.0.0",
        port,
        io_context,
        std::make_unique<session_manager_impl>(),
        std::make_shared<request_handler_impl>());
}

}
