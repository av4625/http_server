#include <algorithm>
#include <stdexcept>

#include "request_impl.hpp"

namespace http
{

request_impl::request_impl(const request_data& data) : data_(data)
{
}

bool request_impl::has_query_param(const std::string& key) const
{
    return std::find_if(
        data_.query_parameters.begin(),
        data_.query_parameters.end(),
        [&key](const query_parameter& q_p){return q_p.name == key;}) !=
            data_.query_parameters.end();
}

const std::string& request_impl::get_query_param(const std::string& key) const
{
    const auto param{std::find_if(
        data_.query_parameters.begin(),
        data_.query_parameters.end(),
        [&key](const query_parameter& q_p){return q_p.name == key;})};

    if (param != data_.query_parameters.end())
    {
        return param->value;
    }
    else
    {
        throw std::invalid_argument("No query parameter with key: " + key);
    }
}

}
