#include "stock_responses.hpp"

#include <string>

#include <boost/lexical_cast.hpp>

#include "header.hpp"
#include "response.hpp"

namespace http
{

namespace
{

const char ok[]{""};
const char created[]{
    "<html>"
    "<head><title>Created</title></head>"
    "<body><h1>201 Created</h1></body>"
    "</html>"};
const char accepted[]{
    "<html>"
    "<head><title>Accepted</title></head>"
    "<body><h1>202 Accepted</h1></body>"
    "</html>"};
const char no_content[]{
    "<html>"
    "<head><title>No Content</title></head>"
    "<body><h1>204 Content</h1></body>"
    "</html>"};
const char multiple_choices[]{
    "<html>"
    "<head><title>Multiple Choices</title></head>"
    "<body><h1>300 Multiple Choices</h1></body>"
    "</html>"};
const char moved_permanently[]{
    "<html>"
    "<head><title>Moved Permanently</title></head>"
    "<body><h1>301 Moved Permanently</h1></body>"
    "</html>"};
const char moved_temporarily[]{
    "<html>"
    "<head><title>Moved Temporarily</title></head>"
    "<body><h1>302 Moved Temporarily</h1></body>"
    "</html>"};
const char not_modified[]{
    "<html>"
    "<head><title>Not Modified</title></head>"
    "<body><h1>304 Not Modified</h1></body>"
    "</html>"};
const char bad_request[]{
    "<html>"
    "<head><title>Bad Request</title></head>"
    "<body><h1>400 Bad Request</h1></body>"
    "</html>"};
const char unauthorized[]{
    "<html>"
    "<head><title>Unauthorized</title></head>"
    "<body><h1>401 Unauthorized</h1></body>"
    "</html>"};
const char forbidden[]{
    "<html>"
    "<head><title>Forbidden</title></head>"
    "<body><h1>403 Forbidden</h1></body>"
    "</html>"};
const char not_found[]{
    "<html>"
    "<head><title>Not Found</title></head>"
    "<body><h1>404 Not Found</h1></body>"
    "</html>"};
const char internal_server_error[]{
    "<html>"
    "<head><title>Internal Server Error</title></head>"
    "<body><h1>500 Internal Server Error</h1></body>"
    "</html>"};
const char not_implemented[]{
    "<html>"
    "<head><title>Not Implemented</title></head>"
    "<body><h1>501 Not Implemented</h1></body>"
    "</html>"};
const char bad_gateway[]{
    "<html>"
    "<head><title>Bad Gateway</title></head>"
    "<body><h1>502 Bad Gateway</h1></body>"
    "</html>"};
const char service_unavailable[]{
    "<html>"
    "<head><title>Service Unavailable</title></head>"
    "<body><h1>503 Service Unavailable</h1></body>"
    "</html>"};

std::string status_code_to_content(const status_code status)
{
    switch (status)
    {
        case status_code::ok:
        {
            return ok;
        }
        case status_code::created:
        {
            return created;
        }
        case status_code::accepted:
        {
            return accepted;
        }
        case status_code::no_content:
        {
            return no_content;
        }
        case status_code::multiple_choices:
        {
            return multiple_choices;
        }
        case status_code::moved_permanently:
        {
            return moved_permanently;
        }
        case status_code::moved_temporarily:
        {
            return moved_temporarily;
        }
        case status_code::not_modified:
        {
            return not_modified;
        }
        case status_code::bad_request:
        {
            return bad_request;
        }
        case status_code::unauthorized:
        {
            return unauthorized;
        }
        case status_code::forbidden:
        {
            return forbidden;
        }
        case status_code::not_found:
        {
            return not_found;
        }
        case status_code::internal_server_error:
        {
            return internal_server_error;
        }
        case status_code::not_implemented:
        {
            return not_implemented;
        }
        case status_code::bad_gateway:
        {
            return bad_gateway;
        }
        case status_code::service_unavailable:
        {
            return service_unavailable;
        }
        default:
        {
            return internal_server_error;
        }
    }
}

}

response stock_reply(const status_code status)
{
    response res;
    res.set_status_code(status);
    res.set_content(status_code_to_content(status));
    res.add_header(
        "Content-Length",
        boost::lexical_cast<std::string>(res.content_length()));
    res.add_header("Content-Type", "text/html");
    res.add_header("Connection", "close");

    return res;
}

}
