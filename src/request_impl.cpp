#include <stdexcept>

#include <http/header_field.hpp>
#include <http/method.hpp>

#include "request_impl.hpp"

namespace http
{

request_impl::request_impl(boost::beast::http::request<
    boost::beast::http::string_body>&& request) :
        request_(std::move(request)),
        url_str_(
            (request_.method() == method::post &&
                request_[field::content_type] == "application/x-www-form-urlencoded") ?
                    (std::string(request_.target()) + "?" + request_.body()) :
                    (std::string(request_.target()))),
        url_(url_str_)
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
