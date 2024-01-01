#include <gtest/gtest.h>

#include <thread>

#include <boost/asio.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>

#include <http/response.hpp>

#include <request_impl.hpp>
#include <server_impl.hpp>
#include <session.hpp>

#include "mocks/request_handler_mock.hpp"
#include "mocks/session_manager_mock.hpp"
#include "test_utilities.hpp"

namespace http
{

namespace
{

const std::string port{"8080"};
const unsigned int version{11};
const boost::beast::http::verb verb{boost::beast::http::verb::get};

struct callback_check
{
    void increment()
    {
        ++i;
    }

    int i = 0;
};

class ServerImplTests : public ::testing::Test
{
protected:
    ServerImplTests() :
        io_context_(std::make_shared<boost::asio::io_context>()),
        session_manager_mock_(std::make_shared<session_manager_mock>()),
        request_handler_mock_(std::make_shared<request_handler_mock>()),
        callback_check_(),
        file_response_callback_(),
        string_response_callback_(),
        t_(),
        server_(std::make_shared<server_impl>(
            "0.0.0.0",
            port,
            io_context_,
            session_manager_mock_,
            request_handler_mock_))
    {
    }

    const std::shared_ptr<boost::asio::io_context> io_context_;
    const std::shared_ptr<session_manager_mock> session_manager_mock_;
    const std::shared_ptr<request_handler_mock> request_handler_mock_;
    callback_check callback_check_;
    std::function<void(const request&, file_response&)> file_response_callback_;
    std::function<void(const request&, string_response&)> string_response_callback_;
    std::thread t_;
    const std::shared_ptr<server> server_;

public:
    void save_file_response_callback(
        const std::string&,
        const method,
        std::function<void(const request&, file_response&)> callback)
    {
        file_response_callback_ = std::move(callback);
    }

    void save_string_response_callback(
        const std::string&,
        const method,
        std::function<void(const request&, string_response&)> callback)
    {
        string_response_callback_ = std::move(callback);
    }

    void save_generic_file_response_callback(
        std::function<void(const request&, file_response&)> callback)
    {
        file_response_callback_ = std::move(callback);
    }

    void save_generic_string_response_callback(
        std::function<void(const request&, string_response&)> callback)
    {
        string_response_callback_ = std::move(callback);
    }
};

}

TEST_F(ServerImplTests, ServeStaticWillPassPathToRequestHandler)
{
    const std::string path{"/path/to/html"};
    EXPECT_CALL(*request_handler_mock_, serve_from_directory(path));
    server_->serve_static(path);
}

TEST_F(ServerImplTests, OnWhenFileResponseWillAddRequestHandler)
{
    const std::string uri{"/endpoint.txt"};
    const method m{method::get};

    EXPECT_CALL(*request_handler_mock_, add_request_handler(
        uri,
        m,
        ::testing::Matcher<std::function<void(const request&, file_response&)> >(
            ::testing::_))).WillOnce(::testing::Invoke(
                this, &ServerImplTests::save_file_response_callback));

    server_->on(
        uri,
        m,
        [this](const request&, file_response&){callback_check_.increment();});

    file_response res(version, true);
    file_response_callback_(
        request_impl(
            boost::beast::http::request<boost::beast::http::string_body>(
                verb, "/", version)),
        res);

    EXPECT_EQ(1, callback_check_.i);
}

TEST_F(ServerImplTests, OnWhenStringResponseWillAddRequestHandler)
{
    const std::string uri{"/endpoint.txt"};
    const method m{method::get};

    EXPECT_CALL(*request_handler_mock_, add_request_handler(
        uri,
        m,
        ::testing::Matcher<std::function<void(const request&, string_response&)> >(
            ::testing::_))).WillOnce(::testing::Invoke(
                this, &ServerImplTests::save_string_response_callback));

    server_->on(
        uri,
        m,
        [this](const request&, string_response&){callback_check_.increment();});

    string_response res(version, true);
    string_response_callback_(
        request_impl(
            boost::beast::http::request<boost::beast::http::string_body>(
                verb, "/", version)),
        res);

    EXPECT_EQ(1, callback_check_.i);
}

TEST_F(ServerImplTests,
    AddGenericRequestHandlerWhenFileResponseWillAddRequestHandler)
{
    EXPECT_CALL(*request_handler_mock_, add_generic_request_handler(
        ::testing::Matcher<std::function<void(const request&, file_response&)> >(
            ::testing::_))).WillOnce(::testing::Invoke(
                this, &ServerImplTests::save_generic_file_response_callback));

    server_->add_generic_request_handler(
        [this](const request&, file_response&){callback_check_.increment();});

    file_response res(version, true);
    file_response_callback_(
        request_impl(
            boost::beast::http::request<boost::beast::http::string_body>(
                verb, "/", version)),
        res);

    EXPECT_EQ(1, callback_check_.i);
}

TEST_F(ServerImplTests,
    AddGenericRequestHandlerWhenStringResponseWillAddRequestHandler)
{
    EXPECT_CALL(*request_handler_mock_, add_generic_request_handler(
        ::testing::Matcher<std::function<void(const request&, string_response&)> >(
            ::testing::_))).WillOnce(::testing::Invoke(
                this, &ServerImplTests::save_generic_string_response_callback));

    server_->add_generic_request_handler(
        [this](const request&, string_response&){callback_check_.increment();});

    string_response res(version, true);
    string_response_callback_(
        request_impl(
            boost::beast::http::request<boost::beast::http::string_body>(
                verb, "/", version)),
        res);

    EXPECT_EQ(1, callback_check_.i);
}

TEST_F(ServerImplTests, BodyLimitWillSetBodyLimitForSession)
{
    // Save the session when its created
    std::shared_ptr<session> session;
    EXPECT_CALL(*session_manager_mock_, stop(::testing::NotNull()));
    EXPECT_CALL(*session_manager_mock_, start(::testing::NotNull()))
        .WillOnce(::testing::Invoke(
            [&session](const std::shared_ptr<http::session> &s)
            {
                session = s;
                session->run();
            }));

    // Set the body limit
    server_->body_limit(10);
    server_->start_server();

    /* Run the io context on another thread so I can call synchronous "client"
       calls on this thread */
    t_ = std::thread([this](){io_context_->run();});

    boost::asio::ip::tcp::resolver resolver(*io_context_);
    boost::beast::tcp_stream stream(*io_context_);
    const auto results{resolver.resolve("localhost", port)};
    // Synchronous connect so that we block until the session is running
    stream.connect(results);

    const std::string body{"413"};
    const boost::beast::http::status status{
        boost::beast::http::status::payload_too_large};

    boost::beast::http::response<boost::beast::http::string_body> response{
        status, version};
    response.keep_alive(false);
    response.set(
        boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
    response.set(boost::beast::http::field::content_type, "text/html");
    response.body() = body;
    response.prepare_payload();

    const std::string endpoint{"/index.html"};

    EXPECT_CALL(*request_handler_mock_, body_limit_reached(
        RequestChecker(endpoint, version, verb)))
            .WillOnce(::testing::Return(::testing::ByMove(response)));

    boost::beast::http::request<boost::beast::http::string_body> request{
        verb, endpoint, version};

    request.keep_alive(false);
    request.set(
        boost::beast::http::field::user_agent, BOOST_BEAST_VERSION_STRING);
    request.body() = "12345678901";
    request.prepare_payload();

    boost::beast::http::write(stream, request);

    boost::beast::http::response<boost::beast::http::dynamic_body> res;
    boost::beast::flat_buffer buffer;

    boost::beast::http::read(stream, buffer, res);

    EXPECT_EQ(status, res.result());
    EXPECT_EQ(version, res.version());
    EXPECT_EQ(false, res.keep_alive());
    EXPECT_EQ(body, boost::beast::buffers_to_string(res.body().data()));

    EXPECT_CALL(*session_manager_mock_, stop_all());

    /* Posting the end server, as the the client is synchronous and the session
       will be finished by this point, then this is posted, it should guarantee
       that the server has responded */
    boost::asio::post(
        *io_context_,
        [this]{server_->end_server();});

    t_.join();
}

TEST_F(ServerImplTests, EndServerWhenServerStartedWillStopServer)
{
    EXPECT_CALL(*session_manager_mock_, stop_all());

    server_->start_server();

    /* Posting the end server, as the server is started first, then this is
       posted, it should guarantee that the server has started and run is
       blocking on it before it is being ended */
    boost::asio::post(
        *io_context_,
        [this]{server_->end_server();});

    io_context_->run();
}

TEST_F(ServerImplTests, EndServerWillCloseAcceptorAndStopAllConnections)
{
    EXPECT_CALL(*session_manager_mock_, stop_all());
    server_->end_server();
}

TEST_F(ServerImplTests, StartServerWhenConnectionWillStartSession)
{
    EXPECT_CALL(*session_manager_mock_, stop_all());
    EXPECT_CALL(*session_manager_mock_, start(::testing::NotNull()))
        .WillOnce([this](){server_->end_server();});

    server_->start_server();

    boost::asio::ip::tcp::resolver resolver(*io_context_);
    boost::beast::tcp_stream stream(*io_context_);
    const auto results{resolver.resolve("localhost", port)};
    stream.connect(results);

    io_context_->run();
}

TEST_F(ServerImplTests, ResetWillCallRequestHandlerReset)
{
    EXPECT_CALL(*request_handler_mock_, reset());
    server_->reset();
}

}
