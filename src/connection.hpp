#ifndef HTTP_CONNECTION_HPP
#define HTTP_CONNECTION_HPP

#include <boost/asio.hpp>
#include <boost/noncopyable.hpp>

namespace http
{

// Represents a single connection from a client.
class connection : private boost::noncopyable
{
public:
    virtual ~connection() = default;

    // Get the socket associated with the connection.
    virtual boost::asio::ip::tcp::socket& socket() = 0;

    // Start the first asynchronous operation for the connection.
    virtual void start() = 0;

    // Stop all asynchronous operations associated with the connection.
    virtual void stop() = 0;
};

}

#endif
