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
            std::function<void(const request&, response&)>
        ),
        (override));

    MOCK_METHOD(
        void, handle_request, (const request_data&, response&), (const, override));
};

}

#endif
