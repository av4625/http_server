#ifndef HTTP_STATUS_CODE_HPP
#define HTTP_STATUS_CODE_HPP

#include <boost/asio/buffer.hpp>

namespace http
{

enum status_code
{
    ok = 200,
    created = 201,
    accepted = 202,
    no_content = 204,
    multiple_choices = 300,
    moved_permanently = 301,
    moved_temporarily = 302,
    not_modified = 304,
    bad_request = 400,
    unauthorized = 401,
    forbidden = 403,
    not_found = 404,
    internal_server_error = 500,
    not_implemented = 501,
    bad_gateway = 502,
    service_unavailable = 503
};

boost::asio::const_buffer http_status_code_to_status_buffer(status_code status);

}

#endif
