#ifndef HTTP_REQUEST_DATA_HPP
#define HTTP_REQUEST_DATA_HPP

#include <string>
#include <vector>

#include "header.hpp"
#include "query_parameter.hpp"

namespace http
{

// A request received from a client.
struct request_data
{
    std::string method;
    std::string uri;
    std::vector<query_parameter> query_parameters;
    int http_version_major;
    int http_version_minor;
    std::vector<header> headers;
};

}

#endif
