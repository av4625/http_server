#include <gtest/gtest.h>

#include <utility>

#include <method.hpp>

namespace http
{

namespace
{

class MethodTests : public ::testing::TestWithParam<
    std::pair<method, std::string> >
{
};

}

TEST(MethodTest, StringToMethodWhenInvalidStringWillThrow)
{
    EXPECT_THROW(string_to_method("MAYBEGET"), std::invalid_argument);
}

TEST_P(MethodTests, StringToMethodWillReturnCorrectMethod)
{
    EXPECT_EQ(GetParam().first, string_to_method(GetParam().second));
}

INSTANTIATE_TEST_SUITE_P(
    Values,
    MethodTests,
    ::testing::Values(
        std::make_pair(method::GET, "GET"),
        std::make_pair(method::HEAD, "HEAD"),
        std::make_pair(method::POST, "POST"),
        std::make_pair(method::PUT, "PUT"),
        std::make_pair(method::DELETE, "DELETE"),
        std::make_pair(method::CONNECT, "CONNECT"),
        std::make_pair(method::OPTIONS, "OPTIONS"),
        std::make_pair(method::TRACE, "TRACE"),
        std::make_pair(method::PATCH, "PATCH")));

}
