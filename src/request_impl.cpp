#include <stdexcept>

#include <http/header_field.hpp>
#include <http/method.hpp>

#include "request_impl.hpp"

namespace http
{

request_impl::request_impl(boost::beast::http::request<
    boost::beast::http::string_body>&& request) :
        request_(std::move(request)),
        uri_((request_.method() == method::post &&
            request_[field::content_type] == "application/x-www-form-urlencoded") ?
                (std::string(request_.target()) + "?" + request_.body()) :
                (std::string(request_.target()))),
        query_parameters_(std::nullopt)
{
}

method request_impl::get_method() const
{
    return request_.method();
}

std::string request_impl::get_endpoint() const
{
    return uri_.getPath();
}

bool request_impl::has_query_param(const std::string& key) const
{
    if (!query_parameters_)
    {
        query_parameters_ = uri_.getQueryParameters();
    }

    return std::any_of(
        query_parameters_->cbegin(),
        query_parameters_->cend(),
        [&key](const std::pair<std::string, std::string>& param)
        {
            return param.first == key;
        }
    );
}

std::string request_impl::get_query_param(const std::string& key) const
{
    if (!query_parameters_)
    {
        query_parameters_ = uri_.getQueryParameters();
    }

    const auto param{std::find_if(
        query_parameters_->cbegin(),
        query_parameters_->cend(),
        [&key](const std::pair<std::string, std::string>& param)
        {
            return param.first == key;
        }
    )};

    if (param != query_parameters_->cend())
    {
        return param->second;
    }

    throw std::invalid_argument(key + " does not exist as a query parameter");
}

bool request_impl::has_header(const std::string& name) const
{
    return has_header_(name);
}

bool request_impl::has_header(const field name) const
{
    return has_header_(name);
}

std::string request_impl::get_header_value(const std::string& name) const
{
    return get_header_value_(name);
}

std::string request_impl::get_header_value(const field name) const
{
    return get_header_value_(name);
}

const std::string& request_impl::body() const
{
    return request_.body();
}

}
