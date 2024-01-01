#ifndef HTTP_REQUEST_HANDLER_MOCK_HPP
#define HTTP_REQUEST_HANDLER_MOCK_HPP

#include <gmock/gmock.h>

#include <request_handler.hpp>

namespace http
{

class request_handler_mock : public request_handler
{
public:
    MOCK_METHOD(void, serve_from_directory, (const std::string&), (override));

    MOCK_METHOD(
        void,
        add_request_handler,
        (
            const std::string&,
            method,
            std::function<void(const request&, file_response&)>
        ),
        (override));

    MOCK_METHOD(
        void,
        add_request_handler,
        (
            const std::string&,
            method,
            std::function<void(const request&, string_response&)>
        ),
        (override));

    MOCK_METHOD(
        void,
        add_generic_request_handler,
        (std::function<void(const request&, file_response&)>),
        (override));

    MOCK_METHOD(
        void,
        add_generic_request_handler,
        (std::function<void(const request&, string_response&)>),
        (override));

    MOCK_METHOD(
        boost::beast::http::message_generator,
        handle_request,
        (boost::beast::http::request<boost::beast::http::string_body>&&),
        (const, override));

    MOCK_METHOD(
        boost::beast::http::message_generator,
        body_limit_reached,
        (boost::beast::http::request<boost::beast::http::string_body>&&),
        (const, override));

    MOCK_METHOD(void, reset, (), (override));
};

}

#endif
