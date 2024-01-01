#ifndef HTTP_REQUEST_HANDLER_HPP
#define HTTP_REQUEST_HANDLER_HPP

#include <string>

#include <boost/beast/http/message.hpp>
#include <boost/beast/http/message_generator.hpp>
#include <boost/beast/http/string_body.hpp>
#include <boost/noncopyable.hpp>

#include <http/method.hpp>

#include "file_response_fwd.hpp"
#include "request_fwd.hpp"
#include "string_response_fwd.hpp"

namespace http
{

// The common handler for all incoming requests.
class request_handler : private boost::noncopyable
{
public:
    virtual ~request_handler() = default;

    /**
     * @brief Serve files from a directory
     *        This is not thread safe and needs to be called when the server is
     *        not running
     *
     * @param[in] doc_root The path to the files to serve
     **/
    virtual void serve_from_directory(const std::string& doc_root) = 0;

    /**
     * @brief Add a custom request handler for responding with a file body.
     *        This is thread safe and new handlers can be added while the server
     *        is running.
     *
     * @param[in] uri The endpoint to handle
     * @param[in] method The method to handle
     * @param[in] callback The handler for the endpoint and method
     **/
    virtual void add_request_handler(
        const std::string& uri,
        method method,
        std::function<void(const request&, file_response&)> callback) = 0;

    /**
     * @brief Add a custom request handler for responding with a string body.
     *        This is thread safe and new handlers can be added while the server
     *        is running.
     *
     * @param[in] uri The endpoint to handle
     * @param[in] method The method to handle
     * @param[in] callback The handler for the endpoint and method
     **/
    virtual void add_request_handler(
        const std::string& uri,
        method method,
        std::function<void(const request&, string_response&)> callback) = 0;

    /**
     * @brief Add a generic request handler for responding with a file body.
     *        If a request doesn't match any exiting handler or there is no
     *        static file for it this handler will be called.
     *        This is not thread safe and needs to be called when the server is
     *        not running
     *
     * @param[in] callback The handler to call for any request that doesn't
     *                     match an existing handler
     **/
    virtual void add_generic_request_handler(
        std::function<void(const request&, file_response&)> callback) = 0;

    /**
     * @brief Add a generic request handler for responding with a string body.
     *        If a request doesn't match any exiting handler or there is no
     *        static file for it this handler will be called.
     *        This is not thread safe and needs to be called when the server is
     *        not running
     *
     * @param[in] callback The handler to call for any request that doesn't
     *                     match an existing handler
     **/
    virtual void add_generic_request_handler(
        std::function<void(const request&, string_response&)> callback) = 0;

    /**
     * @brief Handle a request and produce a response.
     *        This is thread safe so multiple requests can be trying to access
     *        the same custom handler at the same time, but the same custom
     *        handler will not be called at the same time from different
     *        threads.
     *
     * @param[in] request The request to handle
     *
     * @return The response to send as a message generator
     **/
    virtual boost::beast::http::message_generator handle_request(
        boost::beast::http::request<
            boost::beast::http::string_body>&& request) const = 0;

    /**
     * @brief Return the response for when the request is too large
     *
     * @param[in] request The request to handle
     *
     * @return The response to send as a message generator
     **/
    virtual boost::beast::http::message_generator body_limit_reached(
        boost::beast::http::request<
            boost::beast::http::string_body>&& request) const = 0;

    /**
     * @brief Clear all custom request handlers and reset server from directory
     *        path.
     *        This is not thread safe and needs to be called after the server
     *        has been ended.
     **/
    virtual void reset() = 0;
};

}

#endif
