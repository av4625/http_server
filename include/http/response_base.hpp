#ifndef HTTP_RESPONSE_BASE_HPP
#define HTTP_RESPONSE_BASE_HPP

#include <boost/beast/http/message.hpp>

#include "response.hpp"

namespace http
{

template <typename T>
class response_base : public response
{
public:
    response_base(const unsigned int version, const bool keep_alive)
    {
        response_.version(version);
        response_.keep_alive(keep_alive);
        // Default to ok status
        response_.result(status_code::ok);
    }

    void set_status_code(const status_code status) override
    {
        response_.result(status);
    }

    void add_header(const field field, const std::string& value) override
    {
        response_.set(field, value);
    }

    void calculate_and_set_content_length() override
    {
        response_.prepare_payload();
    }

    operator boost::beast::http::message_generator() override
    {
        return std::move(response_);
    }

    operator boost::beast::http::response<T>&&()
    {
        return std::move(response_);
    }

protected:
    boost::beast::http::response<T> response_;
};

}

#endif
