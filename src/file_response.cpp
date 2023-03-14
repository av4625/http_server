#include <http/file_response.hpp>

#include "stock_responses.hpp"

namespace http
{

file_response::file_response(
    const unsigned int version, const bool keep_alive) :
        response_base(version, keep_alive),
        version_(version),
        keep_alive_(keep_alive)
{
}

file_response::operator boost::beast::http::message_generator()
{
    if (response_.body().is_open())
    {
        return std::move(response_);
    }
    else
    {
        return stock_reply(
            boost::beast::http::status::not_found, keep_alive_, version_);
    }
}

void file_response::set_content(const std::string& path)
{
    boost::beast::error_code ec;
    response_.body().open(path.c_str(), boost::beast::file_mode::scan, ec);
}

}
