#ifndef HTTP_REQUEST_DATA_HPP
#define HTTP_REQUEST_DATA_HPP

#include <string>
#include <unordered_map>
#include <vector>

#include "header.hpp"

namespace http
{

// A request received from a client.
struct request_data
{
    std::string method;
    std::string uri;
    int http_version_major;
    int http_version_minor;
    std::unordered_map<std::string, std::string> query_parameters;
    std::vector<header> headers;
};

}

#endif
