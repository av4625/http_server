#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <boost/beast/version.hpp>

#include <response.hpp>
#include <stock_responses.hpp>

namespace http
{

namespace
{

void expect_reply(
    const status_code status_code,
    const std::string& expected_content,
    const std::string& content_length)
{
    const boost::beast::http::response<boost::beast::http::string_body> response{
        stock_reply(status_code, true, 11)};

    EXPECT_EQ(
        BOOST_BEAST_VERSION_STRING,
        std::string(response[boost::beast::http::field::server]));

    EXPECT_EQ(
        "text/html",
        response[boost::beast::http::field::content_type]);

    EXPECT_EQ(
        content_length,
        response[boost::beast::http::field::content_length]);

    EXPECT_TRUE(response.keep_alive());

    EXPECT_THAT(response.body(), ::testing::HasSubstr(expected_content));
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
        "500 Internal Server Error",
        "109");
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
        std::make_pair(status_code::ok, std::make_pair("", "0")),
        std::make_pair(
            status_code::created,
            std::make_pair("201 Created", "81")),
        std::make_pair(
            status_code::accepted,
            std::make_pair("202 Accepted", "83")),
        std::make_pair(
            status_code::no_content,
            std::make_pair("", "0")),
        std::make_pair(
            status_code::multiple_choices,
            std::make_pair("300 Multiple Choices", "99")),
        std::make_pair(
            status_code::moved_permanently,
            std::make_pair("301 Moved Permanently", "101")),
        std::make_pair(
            status_code::found,
            std::make_pair("302 Moved Temporarily", "101")),
        std::make_pair(
            status_code::not_modified,
            std::make_pair("", "0")),
        std::make_pair(
            status_code::bad_request,
            std::make_pair("400 Bad Request", "89")),
        std::make_pair(
            status_code::unauthorized,
            std::make_pair("401 Unauthorized", "91")),
        std::make_pair(
            status_code::forbidden,
            std::make_pair("403 Forbidden", "85")),
        std::make_pair(
            status_code::not_found,
            std::make_pair("404 Not Found", "85")),
        std::make_pair(
            status_code::internal_server_error,
            std::make_pair("500 Internal Server Error", "109")),
        std::make_pair(
            status_code::not_implemented,
            std::make_pair("501 Not Implemented", "97")),
        std::make_pair(
            status_code::bad_gateway,
            std::make_pair("502 Bad Gateway", "89")),
        std::make_pair(
            status_code::service_unavailable,
            std::make_pair("503 Service Unavailable", "105"))));

}
