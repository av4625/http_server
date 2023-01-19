#ifndef HTTP_RESPONSE_IMPL_HPP
#define HTTP_RESPONSE_IMPL_HPP

#include <string>
#include <vector>

#include <boost/asio/buffer.hpp>

#include "header_fwd.hpp"
#include "status_code.hpp"

namespace http
{

class response
{
public:
    response();

    std::vector<boost::asio::const_buffer> to_buffers();

    void set_status_code(status_code status);

    void add_header(
        const std::string& key, const std::string& value);

    void append_content(const char* content, size_t count);

    void set_content(const std::string& content);

    int content_length() const;

private:
    // The status of the response.
    status_code status_;

    // The headers to be included in the response.
    std::vector<header> headers_;

    // The content to be sent in the response.
    std::string content_;
};

}

#endif
