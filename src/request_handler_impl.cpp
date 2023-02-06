#include "request_handler_impl.hpp"

#include <fstream>

#include <boost/beast/core/string_type.hpp>
#include <boost/beast/http/empty_body.hpp>
#include <boost/beast/http/file_body.hpp>
#include <boost/beast/http/status.hpp>
#include <boost/beast/version.hpp>
#include <boost/url/url_view.hpp>

#include "mime_types.hpp"
#include "response.hpp"
#include "request_impl.hpp"
#include "stock_responses.hpp"

namespace http
{

request_handler_impl::request_handler_impl() :
    doc_root_(""),
    handlers_()
{
}

void request_handler_impl::serve_from_directory(const std::string& doc_root)
{
    doc_root_ = doc_root;
}

void request_handler_impl::add_request_handler(
    const std::string& uri,
    const method method,
    std::function<void(const request&, response&)> callback)
{
    handlers_[{uri, method}] = std::move(callback);
}

boost::beast::http::message_generator request_handler_impl::handle_request(
    boost::beast::http::request<
        boost::beast::http::string_body>&& request) const
{
    // Make sure we can handle the method
    if (request.method() != boost::beast::http::verb::get &&
        request.method() != boost::beast::http::verb::head)
    {
        return stock_reply(
            boost::beast::http::status::bad_request,
            request.keep_alive(),
            request.version());
    }

    // Request path must be absolute and not contain "..".
    if (request.target().empty() ||
        request.target()[0] != '/' ||
        request.target().find("..") != boost::beast::string_view::npos)
    {
        return stock_reply(
            boost::beast::http::status::bad_request,
            request.keep_alive(),
            request.version());
    }

    const boost::urls::url_view url(request.target());
    const std::string endpoint(url.path());

    const auto request_type{std::make_pair(endpoint, request.method())};

    if (handlers_.contains(request_type))
    {
        response res(request.version());
        handlers_.at(request_type)(request_impl(std::move(request)), res);

        return static_cast<boost::beast::http::response<boost::beast::http::string_body> >(res);
    }
    else
    {
        // Build the path to the requested file
        std::string path{doc_root_};
        char constexpr path_separator{'/'};

        if (path.back() == path_separator)
        {
            path.resize(path.size() - 1);
        }

        path.append(endpoint);

        // If path ends in slash (i.e. is a directory) then add "index.html".
        if (path.back() == '/')
        {
            path.append("index.html");
        }

        // Attempt to open the file
        boost::beast::error_code ec;
        boost::beast::http::file_body::value_type body;
        body.open(path.c_str(), boost::beast::file_mode::scan, ec);

        // Handle the case where the file doesn't exist
        if (ec == boost::beast::errc::no_such_file_or_directory)
        {
            return stock_reply(
                boost::beast::http::status::not_found,
                request.keep_alive(),
                request.version());
        }

        // Handle an unknown error
        if (ec)
        {
            return stock_reply(
                boost::beast::http::status::internal_server_error,
                request.keep_alive(),
                request.version());
        }

        // Determine the file extension.
        const std::size_t last_slash_pos{path.find_last_of("/")};
        const std::size_t last_dot_pos{path.find_last_of(".")};
        std::string extension;
        if (last_dot_pos != std::string::npos && last_dot_pos > last_slash_pos)
        {
            extension = path.substr(last_dot_pos + 1);
        }

        // Cache the size since we need it after the move
        // To-Do: Can we get the size without reading the file in?
        const auto size{body.size()};

        // Respond to HEAD request
        if(request.method() == boost::beast::http::verb::head)
        {
            boost::beast::http::response<boost::beast::http::empty_body> response{
                boost::beast::http::status::ok, request.version()};
            response.set(boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
            response.set(boost::beast::http::field::content_type, extension_to_mime_type(extension));
            response.content_length(size);
            response.keep_alive(request.keep_alive());
            return response;
        }

        // Respond to GET request
        boost::beast::http::response<boost::beast::http::file_body> response{
            std::piecewise_construct,
            std::make_tuple(std::move(body)),
            std::make_tuple(boost::beast::http::status::ok, request.version())};
        response.set(boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
        response.set(boost::beast::http::field::content_type, extension_to_mime_type(extension));
        response.content_length(size);
        response.keep_alive(request.keep_alive());

        return response;
    }
}

}
