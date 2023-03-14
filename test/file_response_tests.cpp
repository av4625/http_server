#include <gtest/gtest.h>

#include <fstream>

#include <boost/assign/list_of.hpp>
#include <boost/beast/http/message.hpp>
#include <boost/beast/http/file_body.hpp>

#include <http/file_response.hpp>

#include "test_utilities.hpp"

namespace http
{

namespace
{

class FileResponseTests : public ::testing::Test
{
protected:
    FileResponseTests() : response_(11, true)
    {
    }

    file_response response_;
};

}

TEST_F(FileResponseTests, SetStatusCodeWillSetStatusCode)
{
    response_.set_status_code(status_code::conflict);
    const boost::beast::http::response<boost::beast::http::file_body> res(
        response_);
    EXPECT_EQ(status_code::conflict, res.result());
}

TEST_F(FileResponseTests, AddHeaderWillAddHeader)
{
    response_.add_header(field::age, "test");
    const boost::beast::http::response<boost::beast::http::file_body> res(
        response_);
    EXPECT_EQ("test", res[field::age]);
}

TEST_F(FileResponseTests, SetContentWillSetContent)
{
    const std::string path{"./index.html"};

    {
        std::ofstream ofs(path);
        ofs << "testy";
    }

    response_.set_content(path);
    const boost::beast::http::response<boost::beast::http::file_body> res(
        response_);
    EXPECT_EQ(5, res.body().size());
}

TEST_F(FileResponseTests,
    CalculateAndSetContentLengthWillCalculateTheContentLengthAndSetTheContentLengthHeader)
{
    const std::string path{"./index.html"};

    {
        std::ofstream ofs(path);
        ofs << "testy";
    }

    response_.set_content(path);
    response_.calculate_and_set_content_length();
    const boost::beast::http::response<boost::beast::http::file_body> res(
        response_);
    EXPECT_EQ("5", res[field::content_length]);
}

TEST_F(FileResponseTests,
    FileResponseWhenCastToMessageGeneratorAndContentWillReturnResponse)
{
    const std::string path{"./index.html"};
    const std::string content{"Totally cool HTML"};

    {
        std::ofstream ofs(path);
        ofs << content;
    }

    response_.set_content(path);
    response_.calculate_and_set_content_length();

    expect_buffers_has_substrs(
        std::move(response_),
        has_substrs(boost::assign::list_of(content)));
}

TEST_F(FileResponseTests,
    FileResponseWhenCastToMessageGeneratorAndNoContentWillReturn404)
{
    expect_buffers_has_substrs(
        std::move(response_),
        has_substrs(boost::assign::list_of("404")));
}

}
