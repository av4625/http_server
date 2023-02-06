#include <algorithm>
#include <stdexcept>

#include <boost/url/url_view.hpp>

#include "request_impl.hpp"

namespace http
{

request_impl::request_impl(boost::beast::http::request<
    boost::beast::http::string_body>&& request) :
        request_(std::move(request))
{
}

bool request_impl::has_query_param(const std::string& key) const
{
    const boost::urls::url_view url(request_.target());
    return url.params().contains(key);
}

std::string request_impl::get_query_param(const std::string& key) const
{
    const boost::urls::url_view url(request_.target());

    const auto param{std::find_if(
        url.params().begin(),
        url.params().end(),
        [&key](const auto& p){return p.key == key;})};

    if (param != url.params().end())
    {
        // -> is deleted
        return (*param).value;
    }

    throw std::invalid_argument(key + " does not exist as a query parameter");
}

}
