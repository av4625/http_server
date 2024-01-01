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

    /**
     * @brief Set the status code for the response.
     *
     * @param status[in] The statuse code to set.
    */
    virtual void set_status_code(const status_code status) = 0;

    /**
     * @brief Add a header to the response.
     *
     * @param field[in] The field for the header.
     * @param value[in] The header value.
    */
    virtual void add_header(const field field, const std::string& value) = 0;

    /**
     * @brief Calculate the content length and add the content length header to
     *        the response. Should be called after the content has been added to
     *        the response.
    */
    virtual void calculate_and_set_content_length() = 0;

    /**
     * @brief Operator to convert the response to a message generator.
    */
    virtual operator boost::beast::http::message_generator() = 0;
};

}

#endif
