#ifndef HTTP_SERVER_FACTORY_HPP
#define HTTP_SERVER_FACTORY_HPP

#include <memory>
#include <string>

#include <boost/asio.hpp>

#include "server_fwd.hpp"

namespace http
{

class server_factory
{
public:
    std::shared_ptr<server> create(
        const std::string& port,
        const std::shared_ptr<boost::asio::io_context>& io_context) const;
};

}

#endif
