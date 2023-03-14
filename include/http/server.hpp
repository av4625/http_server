#ifndef HTTP_SERVER_HPP
#define HTTP_SERVER_HPP

#include <functional>
#include <string>

#include <boost/noncopyable.hpp>

#include "file_response.hpp"
#include "method.hpp"
#include "request.hpp"
#include "string_response.hpp"

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
        std::function<void(const request&, file_response&)> callback) = 0;

    // Provide function to respond to a certain request
    virtual void on(
        const std::string& uri,
        method method,
        std::function<void(const request&, string_response&)> callback) = 0;

    // Reset and clear down all handlers and paths
    virtual void reset() = 0;
};

}

#endif
