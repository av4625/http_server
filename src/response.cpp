#include <http/response.hpp>

namespace http
{

response::response(const unsigned int version)
{
    response_.version(version);
    // Default to ok status
    response_.result(status_code::ok);
}

void response::set_status_code(const status_code status)
{
    response_.result(status);
}

void response::add_header(const field field, const std::string& value)
{
    response_.set(field, value);
}

void response::set_content(const std::string& content)
{
    response_.body() = content;
}

void response::calculate_and_set_content_length()
{
    response_.prepare_payload();
}

response::operator boost::beast::http::message_generator()
{
    return std::move(response_);
}

response::operator boost::beast::http::response<boost::beast::http::string_body>&&()
{
    return std::move(response_);
}

}
