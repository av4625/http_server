#ifndef HTTP_RESPONSE_IMPL_HPP
#define HTTP_RESPONSE_IMPL_HPP

#include <string>

#include <boost/beast/http/message.hpp>
#include <boost/beast/http/message_generator.hpp>
#include <boost/beast/http/string_body.hpp>

#include "header_field.hpp"
#include "status_code.hpp"

namespace http
{

class response
{
public:
    explicit response(unsigned int version);

    void set_status_code(status_code status);

    void add_header(field field, const std::string& value);

    void set_content(const std::string& content);

    void calculate_and_set_content_length();

    operator boost::beast::http::response<boost::beast::http::string_body>() const;

private:
    boost::beast::http::response<boost::beast::http::string_body> response_;
};

}

#endif
