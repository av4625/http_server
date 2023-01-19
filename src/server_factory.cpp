#include <boost/asio.hpp>

#include "connection_manager_impl.hpp"
#include "request_handler_impl.hpp"
#include "server_factory.hpp"
#include "server_impl.hpp"

namespace http
{

std::unique_ptr<server> server_factory::create(
    const std::string& port,
    const std::shared_ptr<boost::asio::io_context>& io_context) const
{
    return std::make_unique<server_impl>(
        "0.0.0.0",
        port,
        io_context,
        std::make_shared<connection_manager_impl>(),
        std::make_shared<request_handler_impl>());
}

}
