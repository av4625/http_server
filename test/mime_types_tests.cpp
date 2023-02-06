#include <gtest/gtest.h>

#include <utility>

#include <mime_types.hpp>

namespace http
{

namespace
{

class MimeTypesTests : public ::testing::TestWithParam<
    std::pair<std::string, std::string> >
{
};

}

TEST_P(MimeTypesTests, ExtensionToMimeTypeWillReturnCorrectMimeType)
{
    EXPECT_EQ(GetParam().second, extension_to_mime_type(GetParam().first));
}

INSTANTIATE_TEST_SUITE_P(
    Values,
    MimeTypesTests,
    ::testing::Values(
        std::make_pair("gif", "image/gif"),
        std::make_pair("htm", "text/html"),
        std::make_pair("html", "text/html"),
        std::make_pair("php", "text/html"),
        std::make_pair("jpg", "image/jpeg"),
        std::make_pair("png", "image/png"),
        std::make_pair("css", "text/css"),
        std::make_pair("js", "text/javascript"),
        std::make_pair("txt", "text/plain"),
        std::make_pair("blah", "text/plain")));

}
