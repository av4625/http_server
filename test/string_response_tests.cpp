#include <gtest/gtest.h>

#include <boost/assign/list_of.hpp>
#include <boost/beast/http/message.hpp>
#include <boost/beast/http/string_body.hpp>

#include <http/string_response.hpp>

#include "test_utilities.hpp"

namespace http
{

namespace
{

class StringResponseTests : public ::testing::Test
{
protected:
    StringResponseTests() : response_(11, true)
    {
    }

    string_response response_;
};

}

TEST_F(StringResponseTests, SetStatusCodeWillSetStatusCode)
{
    response_.set_status_code(status_code::conflict);
    const boost::beast::http::response<boost::beast::http::string_body> res(
        response_);
    EXPECT_EQ(status_code::conflict, res.result());
}

TEST_F(StringResponseTests, AddHeaderWillAddHeader)
{
    response_.add_header(field::age, "test");
    const boost::beast::http::response<boost::beast::http::string_body> res(
        response_);
    EXPECT_EQ("test", res[field::age]);
}

TEST_F(StringResponseTests, SetContentWillSetContent)
{
    response_.set_content("testy");
    const boost::beast::http::response<boost::beast::http::string_body> res(
        response_);
    EXPECT_EQ("testy", res.body());
}

TEST_F(StringResponseTests,
    CalculateAndSetContentLengthWillCalculateTheContentLengthAndSetTheContentLengthHeader)
{
    response_.set_content("testy");
    response_.calculate_and_set_content_length();
    const boost::beast::http::response<boost::beast::http::string_body> res(
        response_);
    EXPECT_EQ("5", res[field::content_length]);
}

TEST_F(StringResponseTests,
    StringResponseWhenCastToMessageGeneratorWillReturnResponse)
{
    const std::string content{"Totally cool HTML"};

    response_.set_content(content);
    response_.calculate_and_set_content_length();

    expect_buffers_has_substrs(
        std::move(response_),
        has_substrs(boost::assign::list_of(content)));
}

}
