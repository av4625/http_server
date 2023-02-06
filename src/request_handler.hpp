#ifndef HTTP_REQUEST_HANDLER_HPP
#define HTTP_REQUEST_HANDLER_HPP

#include <string>

#include <boost/beast/http/message.hpp>
#include <boost/beast/http/message_generator.hpp>
#include <boost/beast/http/string_body.hpp>
#include <boost/noncopyable.hpp>

#include "method.hpp"
#include "request_fwd.hpp"
#include "response_fwd.hpp"

namespace http
{

// The common handler for all incoming requests.
class request_handler : private boost::noncopyable
{
public:
    virtual ~request_handler() = default;

    // Serve files from a directory
    virtual void serve_from_directory(const std::string& doc_root) = 0;

    // Add a custom request handler
    virtual void add_request_handler(
        const std::string& uri,
        method method,
        std::function<void(const request&, response&)> callback) = 0;

    // Handle a request and produce a response.
    virtual boost::beast::http::message_generator handle_request(
        boost::beast::http::request<
            boost::beast::http::string_body>&& request) const = 0;
};

}

#endif
