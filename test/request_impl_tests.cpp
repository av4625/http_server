#include <gtest/gtest.h>

#include <boost/assign.hpp>

#include <request_impl.hpp>

namespace http
{

namespace
{

const std::vector<query_parameter> query_parameters{
    boost::assign::list_of
        (query_parameter{"key1", "value1"})
        (query_parameter{"key2", "value2"})};

const std::vector<header> headers{
    boost::assign::list_of
        (header{"hkey1", "hvalue1"})
        (header{"hkey2", "hvalue2"})};

class RequestImplTests : public ::testing::Test
{
protected:
    RequestImplTests() :
        request_data_{"GET", "/endpoint", query_parameters, 1, 0, headers},
        request_(request_data_)
    {
    }

    const request_data request_data_;
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

TEST_P(RequestImplHasQueryTests, HasQueryParamWhenWillReturnWhetherItExists)
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
