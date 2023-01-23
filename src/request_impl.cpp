#include <stdexcept>

#include "request_impl.hpp"

namespace http
{

request_impl::request_impl(const request_data& data) : data_(data)
{
}

bool request_impl::has_query_param(const std::string& key) const
{
    return data_.query_parameters.contains(key);
}

const std::string& request_impl::get_query_param(const std::string& key) const
{
    try
    {
        return data_.query_parameters.at(key);
    }
    catch(const std::out_of_range&)
    {
        throw std::invalid_argument("No query parameter with key: " + key);
    }
}

}
