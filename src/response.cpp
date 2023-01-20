#include "response.hpp"

#include <boost/lexical_cast.hpp>

#include "header.hpp"

namespace http
{

namespace
{

const char name_value_separator[] = { ':', ' ' };
const char crlf[] = { '\r', '\n' };

}

response::response() :
    status_(status_code::ok),
    headers_(),
    content_()
{
}

std::vector<boost::asio::const_buffer> response::to_buffers() const
{
    std::vector<boost::asio::const_buffer> buffers;
    buffers.push_back(http_status_code_to_status_buffer(status_));

    for (std::size_t i = 0; i < headers_.size(); ++i)
    {
        const header& h{headers_[i]};
        buffers.push_back(boost::asio::buffer(h.name));
        buffers.push_back(boost::asio::buffer(name_value_separator));
        buffers.push_back(boost::asio::buffer(h.value));
        buffers.push_back(boost::asio::buffer(crlf));
    }

    buffers.push_back(boost::asio::buffer(crlf));
    buffers.push_back(boost::asio::buffer(content_));

    return buffers;
}

void response::set_status_code(const status_code status)
{
    status_ = status;
}

void response::add_header(
    const std::string& key, const std::string& value)
{
    headers_.push_back(header{key, value});
}

void response::append_content(const char* content, const size_t count)
{
    content_.append(content, count);
}

void response::set_content(const std::string& content)
{
    content_ = content;
}

int response::content_length() const
{
    return content_.size();
}

}
