#include "method.hpp"

#include <unordered_map>

#include <boost/assign.hpp>

namespace http
{

namespace
{

const std::unordered_map<std::string, const method> methods(
    boost::assign::map_list_of
        ("GET", method::GET)
        ("HEAD", method::HEAD)
        ("POST", method::POST)
        ("PUT", method::PUT)
        ("DELETE", method::DELETE)
        ("CONNECT", method::CONNECT)
        ("OPTIONS", method::OPTIONS)
        ("TRACE", method::TRACE)
        ("PATCH", method::PATCH));

}

method string_to_method(const std::string& method)
{
    if (methods.contains(method))
    {
        return methods.at(method);
    }

    throw std::invalid_argument(method + ": Is not a valid HTTP method");
}

}
