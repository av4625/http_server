#ifndef HTTP_REQUEST_HPP
#define HTTP_REQUEST_HPP

#include <string>

namespace http
{

// A class/wrapper to provide unmodifiable access to the request
class request
{
public:
    virtual ~request() = default;

    virtual bool has_query_param(const std::string& key) const = 0;

    virtual std::string get_query_param(const std::string& key) const = 0;
};

}

#endif
