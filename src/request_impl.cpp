#include <stdexcept>

#include "request_impl.hpp"

namespace http
{

request_impl::request_impl(boost::beast::http::request<
    boost::beast::http::string_body>&& request) :
        request_(std::move(request)),
        url_(request_.target())
{
}

bool request_impl::has_query_param(const std::string& key) const
{
    return url_.params().contains(key);
}

std::string request_impl::get_query_param(const std::string& key) const
{
    const auto param{url_.params().find(key)};

    if (param != url_.params().end())
    {
        // -> is deleted
        return (*param).value;
    }

    throw std::invalid_argument(key + " does not exist as a query parameter");
}

}
