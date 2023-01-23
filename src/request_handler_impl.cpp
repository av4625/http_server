#include "request_handler_impl.hpp"

#include <fstream>
#include <string>

#include <boost/lexical_cast.hpp>

#include "mime_types.hpp"
#include "response.hpp"
#include "request_data.hpp"
#include "request_impl.hpp"
#include "status_code.hpp"
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

void request_handler_impl::handle_request(const request_data& req, response& res) const
{
    // Decode url to path.
    std::string request_path;
    if (!url_decode(req.uri, request_path))
    {
        res = stock_reply(status_code::bad_request);
        return;
    }

    // Request path must be absolute and not contain "..".
    if (request_path.empty() ||
        request_path[0] != '/' ||
        request_path.find("..") != std::string::npos)
    {
        res = stock_reply(status_code::bad_request);
        return;
    }

    try
    {
        const auto endpoint{std::make_pair(request_path, string_to_method(req.method))};

        // Custom handler specified for the request
        if (handlers_.contains(endpoint))
        {
            handlers_.at(endpoint)(request_impl(req), res);
        }
        // Have a directory to serve
        else if (!doc_root_.empty())
        {
            // If path ends in slash (i.e. is a directory) then add "index.html".
            if (request_path[request_path.size() - 1] == '/')
            {
                request_path += "index.html";
            }

            // Determine the file extension.
            const std::size_t last_slash_pos = request_path.find_last_of("/");
            const std::size_t last_dot_pos = request_path.find_last_of(".");
            std::string extension;
            if (last_dot_pos != std::string::npos && last_dot_pos > last_slash_pos)
            {
                extension = request_path.substr(last_dot_pos + 1);
            }

            // Open the file to send back.
            const std::string full_path = doc_root_ + request_path;
            std::ifstream is(full_path.c_str(), std::ios::in | std::ios::binary);
            if (!is)
            {
                res = stock_reply(status_code::not_found);
                return;
            }

            // Fill out the response to be sent to the client.
            res.set_status_code(status_code::ok);

            char buf[512];

            while (is.read(buf, sizeof(buf)).gcount() > 0)
            {
                res.append_content(buf, is.gcount());
            }

            res.add_header(
                "Content-Length",
                boost::lexical_cast<std::string>(res.content_length()));
            res.add_header("Content-Type", extension_to_mime_type(extension));
            res.add_header("Connection", "close");
        }
    }
    catch(const std::invalid_argument& e)
    {
        res = stock_reply(status_code::bad_request);
    }
}

bool request_handler_impl::url_decode(const std::string& in, std::string& out)
{
    out.clear();
    out.reserve(in.size());

    for (std::size_t i = 0; i < in.size(); ++i)
    {
        if (in[i] == '%')
        {
            if (i + 3 <= in.size())
            {
                int value = 0;
                std::istringstream is(in.substr(i + 1, 2));

                if (is >> std::hex >> value)
                {
                    out += static_cast<char>(value);
                    i += 2;
                }
                else
                {
                    return false;
                }
            }
            else
            {
                return false;
            }
        }
        else if (in[i] == '+')
        {
            out += ' ';
        }
        else
        {
            out += in[i];
        }
    }

    return true;
}

}
