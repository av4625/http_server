#ifndef HTTP_SERVER_HPP
#define HTTP_SERVER_HPP

#include <cstdint>
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

    /**
     * @brief Start the server
    */
    virtual void start_server() = 0;

    /**
     * @brief End the server
    */
    virtual void end_server() = 0;

    /**
     * @brief Serve files from a directory
     *
     * @param path The path to the directory to serve
    */
    virtual void serve_static(const std::string& path) = 0;

    /**
     * @brief Provide function to respond with a file body to a certain request
     *
     * @param uri The endpoint
     * @param method The method of the request
     * @param callback The function to call to build the response
    */
    virtual void on(
        const std::string& uri,
        method method,
        std::function<void(const request&, file_response&)> callback) = 0;

    /**
     * @brief Provide function to respond with a string body to a certain
     *        request
     *
     * @param uri The endpoint
     * @param method The method of the request
     * @param callback The function to call to build the response
    */
    virtual void on(
        const std::string& uri,
        method method,
        std::function<void(const request&, string_response&)> callback) = 0;

    /**
     * @brief Provide a body limit in bytes for incoming requests
     *
     * @param limit The limit in bytes
    */
    virtual void body_limit(std::uint64_t limit) = 0;

    /**
     * @brief Reset and clear down all handlers and paths
    */
    virtual void reset() = 0;
};

}

#endif
