#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <algorithm>
#include <functional>
#include <iterator>
#include <utility>

#include <boost/asio/buffers_iterator.hpp>
#include <boost/lexical_cast.hpp>

#include <header.hpp>
#include <response.hpp>
#include <stock_responses.hpp>

namespace http
{

namespace
{

void expect_reply(
    const status_code status_code,
    const std::string& expected_status,
    const std::string& content)
{
    const auto res{stock_reply(status_code)};
    const auto buffers{res.to_buffers()};

    ASSERT_EQ(15, buffers.size());

    std::vector<std::string> response_text;
    response_text.resize(buffers.size(), "");

    for (unsigned int i = 0; i < buffers.size(); ++i)
    {
        std::copy(
            boost::asio::buffers_begin(buffers[i]),
            boost::asio::buffers_begin(buffers[i]) + buffers[i].size(),
            std::back_inserter(response_text[i]));
    }

    auto has_substr{[](const std::string& needle, const std::string& haystack)
    {
        if (haystack.find(needle) != std::string::npos)
        {
            return true;
        }

        return false;
    }};

    // Status
    const auto status{std::find_if(
        response_text.begin(),
        response_text.end(),
        std::bind(has_substr, expected_status, std::placeholders::_1))};

    EXPECT_NE(response_text.end(), status);

    // Remove as content substring could be the same
    status->erase();

    // Content
    std::vector<std::string>::iterator content_it;

    if (status_code == status_code::ok)
    {
        content_it = std::find(
            response_text.begin(), response_text.end(), content);

        ASSERT_NE(content_it, response_text.end());
    }
    else
    {
        content_it = std::find_if(
            response_text.begin(),
            response_text.end(),
            std::bind(has_substr, content, std::placeholders::_1));

        ASSERT_NE(content_it, response_text.end());
    }

    // Content length key
    EXPECT_NE(
        response_text.end(),
        std::find(
            response_text.begin(), response_text.end(), "Content-Length"));

    // Content length value
    EXPECT_NE(
        response_text.end(),
        std::find(
            response_text.begin(),
            response_text.end(),
            boost::lexical_cast<std::string>(content_it->size())));

    // Content type key
    EXPECT_NE(
        response_text.end(),
        std::find(
            response_text.begin(), response_text.end(), "Content-Type"));

    // Content type value
    EXPECT_NE(
        response_text.end(),
        std::find(
            response_text.begin(), response_text.end(), "text/html"));

    // Connection key
    EXPECT_NE(
        response_text.end(),
        std::find(
            response_text.begin(), response_text.end(), "Connection"));

    // Connection value
    EXPECT_NE(
        response_text.end(),
        std::find(
            response_text.begin(), response_text.end(), "close"));
}

class StockResponsesTests : public ::testing::TestWithParam<
    // <status_code, <status, content>>
    std::pair<status_code, std::pair<std::string, std::string> > >
{
};

}

TEST(StockResponsesTests,
    StockReplyWhenInvalidStatusWillReturnInternalServerError)
{
    expect_reply(
        static_cast<status_code>(999),
        "HTTP/1.0 500 Internal Server Error",
        "500 Internal Server Error");
}

/* Not testing format of the response here
   Current impl sort of relies on knowing the format of the buffers :/ */
TEST_P(StockResponsesTests, StockReplyWillReturnValidReply)
{
    expect_reply(
        GetParam().first,
        GetParam().second.first,
        GetParam().second.second);
}

INSTANTIATE_TEST_SUITE_P(
    Values,
    StockResponsesTests,
    ::testing::Values(
        std::make_pair(status_code::ok, std::make_pair("HTTP/1.0 200 OK", "")),
        std::make_pair(
            status_code::created,
            std::make_pair("HTTP/1.0 201 Created", "201 Created")),
        std::make_pair(
            status_code::accepted,
            std::make_pair("HTTP/1.0 202 Accepted", "202 Accepted")),
        std::make_pair(
            status_code::no_content,
            std::make_pair("HTTP/1.0 204 No Content", "204 No Content")),
        std::make_pair(
            status_code::multiple_choices,
            std::make_pair(
                "HTTP/1.0 300 Multiple Choices", "300 Multiple Choices")),
        std::make_pair(
            status_code::moved_permanently,
            std::make_pair(
                "HTTP/1.0 301 Moved Permanently", "301 Moved Permanently")),
        std::make_pair(
            status_code::moved_temporarily,
            std::make_pair(
                "HTTP/1.0 302 Moved Temporarily", "302 Moved Temporarily")),
        std::make_pair(
            status_code::not_modified,
            std::make_pair("HTTP/1.0 304 Not Modified", "304 Not Modified")),
        std::make_pair(
            status_code::bad_request,
            std::make_pair("HTTP/1.0 400 Bad Request", "400 Bad Request")),
        std::make_pair(
            status_code::unauthorized,
            std::make_pair("HTTP/1.0 401 Unauthorized", "401 Unauthorized")),
        std::make_pair(
            status_code::forbidden,
            std::make_pair("HTTP/1.0 403 Forbidden", "403 Forbidden")),
        std::make_pair(
            status_code::not_found,
            std::make_pair("HTTP/1.0 404 Not Found", "404 Not Found")),
        std::make_pair(
            status_code::internal_server_error,
            std::make_pair(
                "HTTP/1.0 500 Internal Server Error",
                "500 Internal Server Error")),
        std::make_pair(
            status_code::not_implemented,
            std::make_pair(
                "HTTP/1.0 501 Not Implemented", "501 Not Implemented")),
        std::make_pair(
            status_code::bad_gateway,
            std::make_pair("HTTP/1.0 502 Bad Gateway", "502 Bad Gateway")),
        std::make_pair(
            status_code::service_unavailable,
            std::make_pair(
                "HTTP/1.0 503 Service Unavailable",
                "503 Service Unavailable"))));

}
