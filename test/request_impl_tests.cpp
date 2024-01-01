#include <gtest/gtest.h>

#include <optional>

#include <boost/beast/http/message.hpp>
#include <boost/beast/http/string_body.hpp>

#include <http/header_field.hpp>
#include <http/method.hpp>

#include <request_impl.hpp>

namespace http
{

namespace
{

struct request_params
{
    request_params(
        const boost::beast::http::verb method,
        const std::string& endpoint,
        const int version,
        const std::optional<std::string>& content_type,
        const std::string& body,
        const std::string& key) :
            method(method),
            endpoint(endpoint),
            version(version),
            content_type(content_type),
            body(body),
            key(key)
    {
    }

    const boost::beast::http::verb method;
    const std::string endpoint;
    const int version;
    const std::optional<std::string> content_type;
    const std::string body;
    const std::string key;
};

struct request_params_has : public request_params
{
    request_params_has(
        const boost::beast::http::verb method,
        const std::string& endpoint,
        const int version,
        const std::optional<std::string>& content_type,
        const std::string& body,
        const std::string& key,
        const bool expected) :
            request_params(method, endpoint, version, content_type, body, key),
            expected(expected)
    {
    }

    const bool expected;
};

struct request_params_get : public request_params
{
    request_params_get(
        const boost::beast::http::verb method,
        const std::string& endpoint,
        const int version,
        const std::optional<std::string>& content_type,
        const std::string& body,
        const std::string& key,
        const std::string& value) :
            request_params(method, endpoint, version, content_type, body, key),
            value(value)
    {
    }

    const std::string value;
};

request_impl build_request(
    const boost::beast::http::verb method,
    const std::string& endpoint,
    const int version,
    const std::optional<std::string>& content_type,
    const std::string& body)
{
    auto req{boost::beast::http::request<boost::beast::http::string_body>(
        method, endpoint, version, body)};

    if (content_type)
    {
        req.set(boost::beast::http::field::content_type, *content_type);
    }

    return request_impl{std::move(req)};
}

class RequestImplTests : public ::testing::Test
{
protected:
    RequestImplTests() :
        request_(
            build_request(
                boost::beast::http::verb::get,
                "/endpoint",
                11,
                "text/plain",
                "hello"))
    {
    }

    const request_impl request_;
};

class RequestImplHasQueryTests :
    public ::testing::TestWithParam<request_params_has>
{
};

class RequestImplGetQueryTests :
    public ::testing::TestWithParam<request_params_get>
{
};

class RequestImplGetQueryMissingTests :
    public ::testing::TestWithParam<request_params>
{
};

}

TEST_F(RequestImplTests, GetMethodWillReturnMethod)
{
    EXPECT_EQ(method::get, request_.get_method());
}

TEST_F(RequestImplTests, GetEndpointWillReturnEndpoint)
{
    EXPECT_EQ("/endpoint", request_.get_endpoint());
}

TEST_P(RequestImplHasQueryTests, HasQueryParamWillReturnWhetherItExists)
{
    const request_impl request{build_request(
        GetParam().method,
        GetParam().endpoint,
        GetParam().version,
        GetParam().content_type,
        GetParam().body)};

    EXPECT_EQ(GetParam().expected, request.has_query_param(GetParam().key));
}

INSTANTIATE_TEST_SUITE_P(
    Values,
    RequestImplHasQueryTests,
    ::testing::Values(
        request_params_has(
            boost::beast::http::verb::get,
            "/endpoint?key1=value1&key2=value2",
            11,
            std::nullopt,
            "",
            "key1",
            true
        ),
        request_params_has(
            boost::beast::http::verb::get,
            "/endpoint?key1=value1&key2=value2",
            11,
            std::nullopt,
            "",
            "key2",
            true
        ),
        request_params_has(
            boost::beast::http::verb::get,
            "/endpoint?key1=value1&key2=value2",
            11,
            std::nullopt,
            "",
            "key3",
            false
        ),
        request_params_has(
            boost::beast::http::verb::get,
            "/endpoint?key1=value1&key2=value2",
            11,
            std::nullopt,
            "",
            "",
            false
        ),
        request_params_has(
            boost::beast::http::verb::post,
            "/endpoint",
            11,
            "application/x-www-form-urlencoded",
            "key1=value1&key2=value2",
            "key1",
            true
        ),
        request_params_has(
            boost::beast::http::verb::post,
            "/endpoint",
            11,
            "application/x-www-form-urlencoded",
            "key1=value1&key2=value2",
            "key2",
            true
        ),
        request_params_has(
            boost::beast::http::verb::post,
            "/endpoint",
            11,
            "application/x-www-form-urlencoded",
            "key1=value1&key2=value2",
            "key3",
            false
        ),
        request_params_has(
            boost::beast::http::verb::post,
            "/endpoint",
            11,
            "application/x-www-form-urlencoded",
            "key1=value1&key2=value2",
            "",
            false
        ),
        request_params_has(
            boost::beast::http::verb::post,
            "/endpoint",
            11,
            "text/html",
            "key1=value1&key2=value2",
            "key1",
            false
        ),
        request_params_has(
            boost::beast::http::verb::post,
            "/endpoint",
            11,
            std::nullopt,
            "key1=value1&key2=value2",
            "key1",
            false
        ),
        request_params_has(
            boost::beast::http::verb::head,
            "/endpoint?key1=value1&key2=value2",
            11,
            std::nullopt,
            "",
            "key1",
            true
        ),
        request_params_has(
            boost::beast::http::verb::head,
            "/endpoint?key1=value1&key2=value2",
            11,
            "application/x-www-form-urlencoded",
            "",
            "key1",
            true
        )));

TEST_P(RequestImplGetQueryTests, GetQueryParamWhenParamExistsWillReturnValue)
{
    const request_impl request{build_request(
        GetParam().method,
        GetParam().endpoint,
        GetParam().version,
        GetParam().content_type,
        GetParam().body)};

    EXPECT_EQ(GetParam().value, request.get_query_param(GetParam().key));
}

INSTANTIATE_TEST_SUITE_P(
    Values,
    RequestImplGetQueryTests,
    ::testing::Values(
        request_params_get(
            boost::beast::http::verb::get,
            "/endpoint?key1=value1&key2=value2",
            11,
            std::nullopt,
            "",
            "key1",
            "value1"
        ),
        request_params_get(
            boost::beast::http::verb::get,
            "/endpoint?key1=value1&key2=value2",
            11,
            std::nullopt,
            "",
            "key2",
            "value2"
        ),
        request_params_get(
            boost::beast::http::verb::post,
            "/endpoint",
            11,
            "application/x-www-form-urlencoded",
            "key1=value1&key2=value2",
            "key1",
            "value1"
        ),
        request_params_get(
            boost::beast::http::verb::post,
            "/endpoint",
            11,
            "application/x-www-form-urlencoded",
            "key1=value1&key2=value2",
            "key2",
            "value2"
        ),
        request_params_get(
            boost::beast::http::verb::head,
            "/endpoint?key1=value1&key2=value2",
            11,
            std::nullopt,
            "",
            "key1",
            "value1"
        ),
        request_params_get(
            boost::beast::http::verb::head,
            "/endpoint?key1=value1&key2=value2",
            11,
            "application/x-www-form-urlencoded",
            "",
            "key1",
            "value1"
        )));

TEST_P(RequestImplGetQueryMissingTests, GetQueryParamWhenParamDoesntExistWillThrow)
{
    const request_impl request{build_request(
        GetParam().method,
        GetParam().endpoint,
        GetParam().version,
        GetParam().content_type,
        GetParam().body)};

    EXPECT_THROW(
        request.get_query_param(GetParam().key), std::invalid_argument);
}

INSTANTIATE_TEST_SUITE_P(
    Values,
    RequestImplGetQueryMissingTests,
    ::testing::Values(
        request_params(
            boost::beast::http::verb::get,
            "/endpoint?key1=value1&key2=value2",
            11,
            std::nullopt,
            "",
            "key3"
        ),
        request_params(
            boost::beast::http::verb::post,
            "/endpoint",
            11,
            "application/x-www-form-urlencoded",
            "key1=value1&key2=value2",
            "key3"
        ),
        request_params(
            boost::beast::http::verb::head,
            "/endpoint?key1=value1&key2=value2",
            11,
            std::nullopt,
            "",
            "key3"
        ),
        request_params(
            boost::beast::http::verb::get,
            "/endpoint?key1=value1&key2=value2",
            11,
            std::nullopt,
            "",
            ""
        ),
        request_params(
            boost::beast::http::verb::post,
            "/endpoint",
            11,
            "application/x-www-form-urlencoded",
            "key1=value1&key2=value2",
            ""
        ),
        request_params(
            boost::beast::http::verb::head,
            "/endpoint?key1=value1&key2=value2",
            11,
            std::nullopt,
            "",
            ""
        )));

TEST_F(RequestImplTests, HasHeaderWhenHeaderNotPresentAndStringWillReturnFalse)
{
    EXPECT_FALSE(request_.has_header("test"));
}

TEST_F(RequestImplTests, HasHeaderWhenHeaderPresentAndStringWillReturnTrue)
{
    EXPECT_TRUE(request_.has_header("Content-Type"));
}

TEST_F(RequestImplTests, HasHeaderWhenHeaderNotPresentAndFieldWillReturnFalse)
{
    EXPECT_FALSE(request_.has_header(field::content_disposition));
}

TEST_F(RequestImplTests, HasHeaderWhenHeaderPresentAndFieldWillReturnTrue)
{
    EXPECT_TRUE(request_.has_header(field::content_type));
}

TEST_F(RequestImplTests, GetHeaderValueWhenHeaderNotPresentAndStringWillThrow)
{
    EXPECT_THROW(request_.get_header_value("test"), std::invalid_argument);
}

TEST_F(RequestImplTests,
    GetHeaderValueWhenHeaderPresentAndStringWillReturnValue)
{
    EXPECT_EQ("text/plain", request_.get_header_value("Content-Type"));
}

TEST_F(RequestImplTests, GetHeaderValueWhenHeaderNotPresentAndFieldWillThrow)
{
    EXPECT_THROW(
        request_.get_header_value(field::content_disposition),
        std::invalid_argument);
}

TEST_F(RequestImplTests, GetHeaderValueWhenHeaderPresentAndFieldWillReturnValue)
{
    EXPECT_EQ("text/plain", request_.get_header_value(field::content_type));
}

TEST_F(RequestImplTests, BodyWillReturnBody)
{
    EXPECT_EQ("hello", request_.body());
}

}
