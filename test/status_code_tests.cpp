#include <gtest/gtest.h>

#include <algorithm>
#include <utility>

#include <boost/asio/buffers_iterator.hpp>

#include <status_code.hpp>

namespace http
{

namespace
{

void expect_buffer_equal_to_string(
    const boost::asio::const_buffer& buf,
    const std::string& str)
{
    std::string status;

    std::copy(
        boost::asio::buffers_begin(buf),
        boost::asio::buffers_begin(buf) + buf.size(),
        std::back_inserter(status));

    EXPECT_EQ(str, status);
}

class StatusCodeTests : public ::testing::TestWithParam<
    std::pair<status_code, std::string> >
{
};

}

TEST(StatusCodeTest, StockReplyWhenInvalidEnumWillReturnInternalServerError)
{
    expect_buffer_equal_to_string(
        http_status_code_to_status_buffer(static_cast<status_code>(999)),
        "HTTP/1.0 500 Internal Server Error\r\n");
}

TEST_P(StatusCodeTests, StockReplyWillReturnValidReply)
{
    expect_buffer_equal_to_string(
        http_status_code_to_status_buffer(GetParam().first),
        GetParam().second);
}

INSTANTIATE_TEST_SUITE_P(
    Values,
    StatusCodeTests,
    ::testing::Values(
        std::make_pair(status_code::ok, "HTTP/1.0 200 OK\r\n"),
        std::make_pair(
            status_code::created, "HTTP/1.0 201 Created\r\n"),
        std::make_pair(
            status_code::accepted, "HTTP/1.0 202 Accepted\r\n"),
        std::make_pair(
            status_code::no_content, "HTTP/1.0 204 No Content\r\n"),
        std::make_pair(
            status_code::multiple_choices, "HTTP/1.0 300 Multiple Choices\r\n"),
        std::make_pair(
            status_code::moved_permanently,
            "HTTP/1.0 301 Moved Permanently\r\n"),
        std::make_pair(
            status_code::moved_temporarily,
            "HTTP/1.0 302 Moved Temporarily\r\n"),
        std::make_pair(
            status_code::not_modified, "HTTP/1.0 304 Not Modified\r\n"),
        std::make_pair(
            status_code::bad_request, "HTTP/1.0 400 Bad Request\r\n"),
        std::make_pair(
            status_code::unauthorized, "HTTP/1.0 401 Unauthorized\r\n"),
        std::make_pair(
            status_code::forbidden, "HTTP/1.0 403 Forbidden\r\n"),
        std::make_pair(
            status_code::not_found, "HTTP/1.0 404 Not Found\r\n"),
        std::make_pair(
            status_code::internal_server_error,
            "HTTP/1.0 500 Internal Server Error\r\n"),
        std::make_pair(
            status_code::not_implemented, "HTTP/1.0 501 Not Implemented\r\n"),
        std::make_pair(
            status_code::bad_gateway, "HTTP/1.0 502 Bad Gateway\r\n"),
        std::make_pair(
            status_code::service_unavailable,
            "HTTP/1.0 503 Service Unavailable\r\n")));

}
