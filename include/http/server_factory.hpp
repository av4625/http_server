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
    /**
     * @brief Create a server on a specified port that uses the specified
     *        io context.
     *
     * @param port[in] The port for the server to listen on.
     * @param io_context[in] A shared pointer to the io context to used.
     *
     * @returns A shared pointer to the server.
    */
    std::shared_ptr<server> create(
        const std::string& port,
        const std::shared_ptr<boost::asio::io_context>& io_context) const;
};

}

#endif
