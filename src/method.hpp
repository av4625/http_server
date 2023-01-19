#ifndef HTTP_METHOD_HPP
#define HTTP_METHOD_HPP

#include <string>

namespace http
{

enum class method
{
    GET,
    HEAD,
    POST,
    PUT,
    DELETE,
    CONNECT,
    OPTIONS,
    TRACE,
    PATCH
};

method string_to_method(const std::string& method);

}

#endif
