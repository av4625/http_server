#include <gtest/gtest.h>

#include <boost/beast/http/message.hpp>
#include <boost/beast/http/string_body.hpp>

#include <response.hpp>

namespace http
{

namespace
{

class ResponseTests : public ::testing::Test
{
protected:
    ResponseTests() : response_(11)
    {
    }

    response response_;
};

}

TEST_F(ResponseTests, SetStatusCodeWillSetStatusCode)
{
    response_.set_status_code(status_code::conflict);
    const boost::beast::http::response<boost::beast::http::string_body> res(
        response_);
    EXPECT_EQ(status_code::conflict, res.result());
}

TEST_F(ResponseTests, AddHeaderWillAddHeader)
{
    response_.add_header(field::age, "test");
    const boost::beast::http::response<boost::beast::http::string_body> res(
        response_);
    EXPECT_EQ("test", res[field::age]);
}

TEST_F(ResponseTests, SetContentWillSetContent)
{
    response_.set_content("testy");
    const boost::beast::http::response<boost::beast::http::string_body> res(
        response_);
    EXPECT_EQ("testy", res.body());
}

TEST_F(ResponseTests,
    CalculateAndSetContentLengthWillCalculateTheContentLengthAndSetTheContentLengthHeader)
{
    response_.set_content("testy");
    response_.calculate_and_set_content_length();
    const boost::beast::http::response<boost::beast::http::string_body> res(
        response_);
    EXPECT_EQ("5", res[field::content_length]);
}

}
