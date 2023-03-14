#ifndef HTTP_RESPONSE_HPP
#define HTTP_RESPONSE_HPP

#include <string>

#include <boost/beast/http/message_generator.hpp>

#include "header_field.hpp"
#include "status_code.hpp"

namespace http
{

class response
{
public:
    virtual ~response() = default;

    virtual void set_status_code(const status_code status) = 0;

    virtual void add_header(const field field, const std::string& value) = 0;

    virtual void calculate_and_set_content_length() = 0;

    virtual operator boost::beast::http::message_generator() = 0;
};

}

#endif
