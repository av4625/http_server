#include "status_code.hpp"

#include <string>

namespace http
{

namespace
{

const std::string ok_status_str{"HTTP/1.0 200 OK\r\n"};
const std::string created_status_str{"HTTP/1.0 201 Created\r\n"};
const std::string accepted_status_str{"HTTP/1.0 202 Accepted\r\n"};
const std::string no_content_status_str{"HTTP/1.0 204 No Content\r\n"};
const std::string multiple_choices_status_str{
    "HTTP/1.0 300 Multiple Choices\r\n"};
const std::string moved_permanently_status_str{
    "HTTP/1.0 301 Moved Permanently\r\n"};
const std::string moved_temporarily_status_str{
    "HTTP/1.0 302 Moved Temporarily\r\n"};
const std::string not_modified_status_str{"HTTP/1.0 304 Not Modified\r\n"};
const std::string bad_request_status_str{"HTTP/1.0 400 Bad Request\r\n"};
const std::string unauthorized_status_str{"HTTP/1.0 401 Unauthorized\r\n"};
const std::string forbidden_status_str{"HTTP/1.0 403 Forbidden\r\n"};
const std::string not_found_status_str{"HTTP/1.0 404 Not Found\r\n"};
const std::string internal_server_error_status_str{
    "HTTP/1.0 500 Internal Server Error\r\n"};
const std::string not_implemented_status_str{
    "HTTP/1.0 501 Not Implemented\r\n"};
const std::string bad_gateway_status_str{"HTTP/1.0 502 Bad Gateway\r\n"};
const std::string service_unavailable_status_str{
    "HTTP/1.0 503 Service Unavailable\r\n"};

}

boost::asio::const_buffer http_status_code_to_status_buffer(
    const status_code status)
{
    switch (status)
    {
        case status_code::ok:
        {
            return boost::asio::buffer(ok_status_str);
        }
        case status_code::created:
        {
            return boost::asio::buffer(created_status_str);
        }
        case status_code::accepted:
        {
            return boost::asio::buffer(accepted_status_str);
        }
        case status_code::no_content:
        {
            return boost::asio::buffer(no_content_status_str);
        }
        case status_code::multiple_choices:
        {
            return boost::asio::buffer(multiple_choices_status_str);
        }
        case status_code::moved_permanently:
        {
            return boost::asio::buffer(moved_permanently_status_str);
        }
        case status_code::moved_temporarily:
        {
            return boost::asio::buffer(moved_temporarily_status_str);
        }
        case status_code::not_modified:
        {
            return boost::asio::buffer(not_modified_status_str);
        }
        case status_code::bad_request:
        {
            return boost::asio::buffer(bad_request_status_str);
        }
        case status_code::unauthorized:
        {
            return boost::asio::buffer(unauthorized_status_str);
        }
        case status_code::forbidden:
        {
            return boost::asio::buffer(forbidden_status_str);
        }
        case status_code::not_found:
        {
            return boost::asio::buffer(not_found_status_str);
        }
        case status_code::internal_server_error:
        {
            return boost::asio::buffer(internal_server_error_status_str);
        }
        case status_code::not_implemented:
        {
            return boost::asio::buffer(not_implemented_status_str);
        }
        case status_code::bad_gateway:
        {
            return boost::asio::buffer(bad_gateway_status_str);
        }
        case status_code::service_unavailable:
        {
            return boost::asio::buffer(service_unavailable_status_str);
        }
        default:
        {
            return boost::asio::buffer(internal_server_error_status_str);
        }
    }
}

}
