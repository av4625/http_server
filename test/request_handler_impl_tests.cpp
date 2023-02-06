#include <gtest/gtest.h>

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <functional>

#include <boost/asio/buffers_iterator.hpp>
#include <boost/assign/list_of.hpp>

#include <http/response.hpp>

#include <request_handler_impl.hpp>

namespace http
{

namespace
{

bool needle_in_haystack(const std::string& haystack, const std::string& needle)
{
    if (haystack.find(needle) != std::string::npos)
    {
        return true;
    }

    return false;
}

struct has_substrs
{
public:
    explicit has_substrs(std::vector<std::string>&& needles) :
        needles(std::move(needles)),
        needles_found_(0)
    {
    }

    bool operator()(const std::string& haystack)
    {
        needles_found_ += std::count_if(
            needles.begin(),
            needles.end(),
            std::bind(needle_in_haystack, haystack, std::placeholders::_1));

        if (needles_found_ == needles.size())
        {
            return true;
        }

        return false;
    }

private:
    std::vector<std::string> needles;
    unsigned int needles_found_;
};

class RequestHandlerImplTests : public ::testing::Test
{
protected:
    RequestHandlerImplTests() : request_handler_()
    {
    }

    request_handler_impl request_handler_;

    std::vector<std::string> expect_buffers_has_substrs(
        boost::beast::http::message_generator&& msg,
        has_substrs&& has_substrs)
    {
        boost::system::error_code ec;
        const auto buffer{msg.prepare(ec)};

        std::vector<std::string> response_text;
        response_text.resize(buffer.size(), "");

        for (unsigned int i = 0; i < buffer.size(); ++i)
        {
            std::copy(
                boost::asio::buffers_begin(buffer[i]),
                boost::asio::buffers_begin(buffer[i]) + buffer[i].size(),
                std::back_inserter(response_text[i]));
        }

        const auto it{std::find_if(
            response_text.begin(),
            response_text.end(),
            has_substrs)};

        EXPECT_NE(response_text.end(), it);

        return response_text;
    }

public:
    void custom_handler(
        const std::string& content, const request&, response& res)
    {
        res.set_content(content);
    }
};

class RequestHandlerImplVerbTests :
    public RequestHandlerImplTests,
    public ::testing::WithParamInterface<boost::beast::http::verb>
{
};

class RequestHandlerImplAllVerbTests :
    public RequestHandlerImplTests,
    public ::testing::WithParamInterface<boost::beast::http::verb>
{
};

class RequestHandlerImplInvalidVerbTests :
    public RequestHandlerImplTests,
    public ::testing::WithParamInterface<boost::beast::http::verb>
{
};

class RequestHandlerImplVerbInvalidTargetTests :
    public RequestHandlerImplTests,
    public ::testing::WithParamInterface<
        std::pair<boost::beast::http::verb, std::string> >
{
};

}

TEST_P(RequestHandlerImplInvalidVerbTests,
    HandleRequestWhenVerbNotGetOrHeadWillReturnBadRequest)
{
    request_handler_.serve_from_directory("/");

    boost::beast::http::request<
        boost::beast::http::string_body> request(
            GetParam(), "/", 11);

    auto msg{request_handler_.handle_request(std::move(request))};

    expect_buffers_has_substrs(
        std::move(msg),
        has_substrs(boost::assign::list_of("400")("Bad Request")("1.1")));
}

INSTANTIATE_TEST_SUITE_P(
    Values,
    RequestHandlerImplInvalidVerbTests,
    ::testing::Values(
        boost::beast::http::verb::delete_,
        boost::beast::http::verb::options,
        boost::beast::http::verb::post,
        boost::beast::http::verb::patch,
        boost::beast::http::verb::put,
        boost::beast::http::verb::trace,
        boost::beast::http::verb::connect));

TEST_P(RequestHandlerImplVerbInvalidTargetTests,
    HandleRequestWhenInvalidTargetWillReturnBadRequest)
{
    request_handler_.serve_from_directory("/");

    boost::beast::http::request<
        boost::beast::http::string_body> request(
            GetParam().first, GetParam().second, 11);

    auto msg{request_handler_.handle_request(std::move(request))};

    expect_buffers_has_substrs(
        std::move(msg),
        has_substrs(boost::assign::list_of("400")("Bad Request")("1.1")));
}

INSTANTIATE_TEST_SUITE_P(
    Values,
    RequestHandlerImplVerbInvalidTargetTests,
    ::testing::Values(
        std::make_pair(boost::beast::http::verb::get, ""),
        std::make_pair(boost::beast::http::verb::head, ""),
        std::make_pair(boost::beast::http::verb::get, "a"),
        std::make_pair(boost::beast::http::verb::head, "v"),
        std::make_pair(boost::beast::http::verb::get, "/.."),
        std::make_pair(boost::beast::http::verb::head, "/.."),
        std::make_pair(boost::beast::http::verb::get, "/endpoint/.."),
        std::make_pair(boost::beast::http::verb::head, "/endpoint/..")));

TEST_P(RequestHandlerImplAllVerbTests,
    HandleRequestWhenNoCustomHandlerAndDocRootNotSetWillReturnNotFound)
{
    boost::beast::http::request<
        boost::beast::http::string_body> request(
            GetParam(), "/", 11);

    auto msg{request_handler_.handle_request(std::move(request))};

    expect_buffers_has_substrs(
        std::move(msg),
        has_substrs(boost::assign::list_of("404")("Not Found")("1.1")));
}

TEST_P(RequestHandlerImplAllVerbTests,
    HandleRequestWhenCustomHandlerWillCallHandler)
{
    const std::string content{"custom content"};
    const std::string endpoint{"/endpoint"};

    request_handler_.add_request_handler(
        endpoint,
        GetParam(),
        std::bind(
            &RequestHandlerImplAllVerbTests::custom_handler,
            this,
            content,
            std::placeholders::_1,
            std::placeholders::_2));

    boost::beast::http::request<
        boost::beast::http::string_body> request(GetParam(), endpoint, 11);

    auto msg{request_handler_.handle_request(std::move(request))};

    expect_buffers_has_substrs(
        std::move(msg), has_substrs(boost::assign::list_of(content)));
}

INSTANTIATE_TEST_SUITE_P(
    Values,
    RequestHandlerImplAllVerbTests,
    ::testing::Values(
        boost::beast::http::verb::delete_,
        boost::beast::http::verb::options,
        boost::beast::http::verb::post,
        boost::beast::http::verb::patch,
        boost::beast::http::verb::put,
        boost::beast::http::verb::trace,
        boost::beast::http::verb::connect,
        boost::beast::http::verb::head,
        boost::beast::http::verb::get));

TEST_P(RequestHandlerImplVerbTests,
    HandleRequestWhenFileNotFoundWillReturnNotFound)
{
    request_handler_.serve_from_directory("/");

    boost::beast::http::request<
        boost::beast::http::string_body> request(
            GetParam(), "/", 11);

    auto msg{request_handler_.handle_request(std::move(request))};

    expect_buffers_has_substrs(
        std::move(msg),
        has_substrs(boost::assign::list_of("404")("Not Found")("1.1")));
}

INSTANTIATE_TEST_SUITE_P(
    Values,
    RequestHandlerImplVerbTests,
    ::testing::Values(
        boost::beast::http::verb::get, boost::beast::http::verb::head));

TEST_F(RequestHandlerImplTests,
    HandleRequestWhenGetAndEndpointIsDirectoryWillRespondWithIndexHtml)
{
    const std::string content{"Fancy HTML"};
    const std::filesystem::path index{"./index.html"};

    {
        std::ofstream ofs(index);
        ofs << content;
    }

    request_handler_.serve_from_directory("./");

    boost::beast::http::request<
        boost::beast::http::string_body> request(
            boost::beast::http::verb::get, "/", 11);

    auto msg{request_handler_.handle_request(std::move(request))};

    expect_buffers_has_substrs(
        std::move(msg),
        has_substrs(boost::assign::list_of
            ("200")
            (content.c_str())
            ("1.1")
            ("text/html")
            (std::to_string(content.size()).c_str())));

    std::filesystem::remove(index);
}

TEST_F(RequestHandlerImplTests,
    HandleRequestWhenGetAndEndpointIsFileWillRespondWithFile)
{
    const std::string content{"Fancy CSS"};
    const std::filesystem::path css{"./magic.css"};

    {
        std::ofstream ofs(css);
        ofs << content;
    }

    request_handler_.serve_from_directory("./");

    boost::beast::http::request<
        boost::beast::http::string_body> request(
            boost::beast::http::verb::get, "/magic.css", 11);

    auto msg{request_handler_.handle_request(std::move(request))};

    expect_buffers_has_substrs(
        std::move(msg),
        has_substrs(boost::assign::list_of
            ("200")
            (content.c_str())
            ("1.1")
            ("text/css")
            (std::to_string(content.size()).c_str())));

    std::filesystem::remove(css);
}

TEST_F(RequestHandlerImplTests,
    HandleRequestWhenGetAndRootDoesntEndInSlashWillRespondWithFile)
{
    const std::string content{"Fancy CSS"};
    const std::filesystem::path dir{"./test"};
    const std::string file{"magic.css"};

    std::filesystem::create_directories(dir);

    {
        std::ofstream ofs(dir / file);
        ofs << content;
    }

    request_handler_.serve_from_directory(dir.string());

    boost::beast::http::request<
        boost::beast::http::string_body> request(
            boost::beast::http::verb::get, "/" + file, 10);

    auto msg{request_handler_.handle_request(std::move(request))};

    expect_buffers_has_substrs(
        std::move(msg),
        has_substrs(boost::assign::list_of
            ("200")
            (content.c_str())
            ("1.0")
            ("text/css")
            (std::to_string(content.size()).c_str())));

    std::filesystem::remove_all(dir);
}

TEST_F(RequestHandlerImplTests,
    HandleRequestWhenHeadAndEndpointIsDirectoryWillRespondWithIndexHtmlButWithoutContent)
{
    const std::string content{"Fancy HTML"};
    const std::filesystem::path index{"./index.html"};

    {
        std::ofstream ofs(index);
        ofs << content;
    }

    request_handler_.serve_from_directory("./");

    boost::beast::http::request<
        boost::beast::http::string_body> request(
            boost::beast::http::verb::head, "/", 11);

    auto msg{request_handler_.handle_request(std::move(request))};

    const auto result_text{expect_buffers_has_substrs(
        std::move(msg),
        has_substrs(boost::assign::list_of
            ("200")
            ("1.1")
            ("text/html")
            (std::to_string(content.size()).c_str())))};

    // Ensure content isn't in the response
    const auto it{std::find_if(
        result_text.begin(),
        result_text.end(),
        std::bind(needle_in_haystack, std::placeholders::_1, content))};

    EXPECT_EQ(result_text.end(), it);

    std::filesystem::remove(index);
}

TEST_F(RequestHandlerImplTests, ResetWillClearHandlersAndDirectoryPath)
{
    const std::string content{"Fancy HTML"};
    const std::filesystem::path index{"./index.html"};

    {
        std::ofstream ofs(index);
        ofs << content;
    }

    request_handler_.add_request_handler(
        "./",
        method::get,
        std::bind(
            &RequestHandlerImplAllVerbTests::custom_handler,
            this,
            "custom content",
            std::placeholders::_1,
            std::placeholders::_2));

    request_handler_.serve_from_directory("./");

    request_handler_.reset();

    boost::beast::http::request<
        boost::beast::http::string_body> request(
            boost::beast::http::verb::head, "/", 11);

    auto msg{request_handler_.handle_request(std::move(request))};

    expect_buffers_has_substrs(
        std::move(msg),
        has_substrs(boost::assign::list_of("404")("Not Found")("1.1")));

    std::filesystem::remove(index);
}

}
