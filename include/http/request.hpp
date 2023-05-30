#ifndef HTTP_REQUEST_HPP
#define HTTP_REQUEST_HPP

#include <string>

#include "header_field.hpp"

namespace http
{

// A class/wrapper to provide unmodifiable access to the request
class request
{
public:
    virtual ~request() = default;

    virtual bool has_query_param(const std::string& key) const = 0;

    virtual std::string get_query_param(const std::string& key) const = 0;

    virtual bool has_header(const std::string& name) const = 0;

    virtual bool has_header(field name) const = 0;

    virtual std::string get_header_value(const std::string& name) const = 0;

    virtual std::string get_header_value(field name) const = 0;

    virtual const std::string& body() const = 0;
};

}

#endif
