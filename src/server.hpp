#ifndef HTTP_SERVER_HPP
#define HTTP_SERVER_HPP

#include <functional>
#include <string>

#include <boost/noncopyable.hpp>

#include "request.hpp"
#include "response.hpp"
#include "method.hpp"

namespace http
{

class server : private boost::noncopyable
{
public:
    virtual ~server() = default;

    // Start the server
    virtual void start_server() = 0;

    // End the server
    virtual void end_server() = 0;

    // Serve files from a directory
    virtual void serve_static(const std::string& path) = 0;

    // Provide function to respond to a certain request
    virtual void on(
        const std::string& uri,
        method method,
        std::function<void(const request&, response&)> callback) = 0;
};

}

#endif
