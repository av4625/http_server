#include <gtest/gtest.h>

#include <boost/beast/http/message.hpp>
#include <boost/beast/http/string_body.hpp>

#include <method.hpp>
#include <request_impl.hpp>

namespace http
{

namespace
{

class RequestImplTests : public ::testing::Test
{
protected:
    RequestImplTests() :
        request_(
            boost::beast::http::request<boost::beast::http::string_body>(
                boost::beast::http::verb::get,
                "/endpoint?key1=value1&key2=value2",
                11))
    {
    }

    boost::beast::http::request<boost::beast::http::string_body> boost_request_;
    const request_impl request_;
};

class RequestImplHasQueryTests :
    public RequestImplTests,
    public ::testing::WithParamInterface<std::pair<bool, std::string> >
{
};

class RequestImplGetQueryTests :
    public RequestImplTests,
    public ::testing::WithParamInterface<std::pair<std::string, std::string> >
{
};

}

TEST_P(RequestImplHasQueryTests, HasQueryParamWillReturnWhetherItExists)
{
    EXPECT_EQ(GetParam().first, request_.has_query_param(GetParam().second));
}

INSTANTIATE_TEST_SUITE_P(
    Values,
    RequestImplHasQueryTests,
    ::testing::Values(
        std::make_pair(true, "key1"),
        std::make_pair(true, "key2"),
        std::make_pair(false, "key3"),
        std::make_pair(false, "")));

TEST_P(RequestImplGetQueryTests, GetQueryParamWhenParamExistsWillReturnValue)
{
    EXPECT_EQ(GetParam().first, request_.get_query_param(GetParam().second));
}

INSTANTIATE_TEST_SUITE_P(
    Values,
    RequestImplGetQueryTests,
    ::testing::Values(
        std::make_pair("value1", "key1"),
        std::make_pair("value2", "key2")));

TEST_F(RequestImplTests, GetQueryParamWhenParamDoesntExistWillThrow)
{
    EXPECT_THROW(request_.get_query_param("key3"), std::invalid_argument);
}

}
