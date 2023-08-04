#include <gtest/gtest.h>

#include <thread>

#include <boost/asio.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>

#include <session_impl.hpp>

#include "mocks/request_handler_mock.hpp"
#include "mocks/session_manager_mock.hpp"
#include "test_utilities.hpp"

namespace http
{

namespace
{

const std::string endpoint_target{"/index.html"};
const unsigned int version{11};
const boost::beast::http::verb verb{boost::beast::http::verb::get};
const boost::beast::http::status ok{boost::beast::http::status::ok};
const std::string content{"fancy conent"};

class test_server
{
private:
    boost::asio::ip::tcp::acceptor acceptor_;
};

class SessionImplBase : public ::testing::Test
{
protected:
    explicit SessionImplBase(const std::uint64_t body_limit) :
        io_context_(std::make_shared<boost::asio::io_context>()),
        resolver_(*io_context_),
        acceptor_(*io_context_),
        stream_(*io_context_),
        request_handler_mock_(std::make_shared<request_handler_mock>()),
        session_manager_mock_(std::make_shared<session_manager_mock>()),
        t_(),
        response_(ok, version),
        request_(verb, endpoint_target, version)
    {
        response_.set(boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
        response_.set(boost::beast::http::field::content_type, "text/html");
        response_.body() = content;
        response_.prepare_payload();

        request_.set(
            boost::beast::http::field::user_agent, BOOST_BEAST_VERSION_STRING);
        request_.body() = content;
        request_.prepare_payload();

        /* Setup a dummy "server" to accept to get a valid socket to construct
           the session. */
        const auto endpoints{resolver_.resolve("localhost", "8080")};
        boost::asio::ip::tcp::endpoint endpoint{*endpoints.begin()};

        acceptor_.open(endpoint.protocol());
        acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
        acceptor_.bind(endpoint);
        acceptor_.listen(boost::asio::ip::tcp::acceptor::max_listen_connections);

        // Connect a client
        stream_.async_connect(
            endpoints,
            [](const boost::beast::error_code&, const boost::asio::ip::tcp::endpoint&){});

        // Synchronously accept to get the socket to construct the session
        boost::asio::ip::tcp::socket sock{acceptor_.accept(
            boost::asio::make_strand(*io_context_))};

        session_ = std::make_shared<session_impl>(
            std::move(sock),
            request_handler_mock_,
            session_manager_mock_,
            body_limit);
    }

    const std::shared_ptr<boost::asio::io_context> io_context_;
    boost::asio::ip::tcp::resolver resolver_;
    boost::asio::ip::tcp::acceptor acceptor_;
    boost::beast::tcp_stream stream_;
    const std::shared_ptr<request_handler_mock> request_handler_mock_;
    const std::shared_ptr<session_manager_mock> session_manager_mock_;
    std::thread t_;
    boost::beast::http::response<boost::beast::http::string_body> response_;
    boost::beast::http::request<boost::beast::http::string_body> request_;
    std::shared_ptr<session> session_;

    // Pretend "client" sends request and inspects response
    void send_request_and_expect_response(
        const bool keep_alive,
        const boost::beast::http::status status = boost::beast::http::status::ok)
    {
        boost::beast::http::write(stream_, request_);

        boost::beast::http::response<boost::beast::http::dynamic_body> res;
        boost::beast::flat_buffer buffer;

        boost::beast::http::read(stream_, buffer, res);

        EXPECT_EQ(status, res.result());
        EXPECT_EQ(version, res.version());
        EXPECT_EQ(keep_alive, res.keep_alive());
        EXPECT_EQ(content, boost::beast::buffers_to_string(res.body().data()));
    }

    void finish()
    {
        // Session stops itself
        // Close acceptor (Is this necessary as we accepted synchronously?)
        acceptor_.close();

        /* IO context will exit itself when the session is closed and the
            "server" is no longer accepting, so we can join the thread again*/
        t_.join();
    }
};

class SessionImplTests : public SessionImplBase
{
protected:
    SessionImplTests() : SessionImplBase(1000000)
    {
    }
};

class SessionImplSmallLimitTests : public SessionImplBase
{
protected:
    SessionImplSmallLimitTests() :
        SessionImplBase(1),
        response_413_(boost::beast::http::status::payload_too_large, version)
    {
        response_413_.keep_alive(false);
        response_413_.set(
            boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
        response_413_.set(boost::beast::http::field::content_type, "text/html");
        response_413_.body() = content;
        response_413_.prepare_payload();
    }

    boost::beast::http::response<boost::beast::http::string_body> response_413_;
};

}

TEST_F(SessionImplTests,
    RunWhenNotKeepAliveWillReadARequestAndSendAResponseAndClose)
{
    // Start the session, this will cause an async "loop"
    session_->run();

    /* Run the io context on another thread so I can call synchronous "client"
       calls on this thread */
    t_ = std::thread([this](){io_context_->run();});

    EXPECT_CALL(*session_manager_mock_, stop(session_));

    response_.keep_alive(false);

    EXPECT_CALL(*request_handler_mock_, handle_request(
        RequestChecker(endpoint_target, version, verb)))
            .WillOnce(::testing::Return(::testing::ByMove(response_)));

    send_request_and_expect_response(false);

    finish();

    /* GMock keeps session_ alive as it is checked as a param to stop, this is
       required to make it think its not leaking it
       https://stackoverflow.com/questions/10286514/why-is-googlemock-leaking-my-shared-ptr */
    ::testing::Mock::VerifyAndClearExpectations(session_manager_mock_.get());
}

TEST_F(SessionImplTests, StopWhenNotKeepAliveWillAttemptShutdownButHaveNoEffect)
{
    // Start the session, this will cause an async "loop"
    session_->run();

    /* Run the io context on another thread so I can call synchronous "client"
       calls on this thread */
    t_ = std::thread([this](){io_context_->run();});

    EXPECT_CALL(*session_manager_mock_, stop(session_));

    response_.keep_alive(false);

    EXPECT_CALL(*request_handler_mock_, handle_request(
        RequestChecker(endpoint_target, version, verb)))
            .WillOnce(::testing::Return(::testing::ByMove(response_)));

    send_request_and_expect_response(false);

    finish();

    /* GMock keeps session_ alive as it is checked as a param to stop, this is
       required to make it think its not leaking it
       https://stackoverflow.com/questions/10286514/why-is-googlemock-leaking-my-shared-ptr */
    ::testing::Mock::VerifyAndClearExpectations(session_manager_mock_.get());

    // Stop the session
    session_->stop();
}

TEST_F(SessionImplTests,
    StartWhenKeepAliveWillReadAnotherRequestAndCanBeShutdown)
{
    // Start the session, this will cause an async "loop"
    session_->run();

    /* Run the io context on another thread so I can call synchronous "client"
       calls on this thread */
    t_ = std::thread([this](){io_context_->run();});

    response_.keep_alive(true);

    EXPECT_CALL(*request_handler_mock_, handle_request(
        RequestChecker(endpoint_target, version, verb)))
            .WillOnce(::testing::Return(::testing::ByMove(response_)));

    send_request_and_expect_response(true);

    EXPECT_CALL(*session_manager_mock_, stop(session_));

    // Stop the session
    session_->stop();
    finish();

    /* GMock keeps session_ alive as it is checked as a param to stop, this is
       required to make it think its not leaking it
       https://stackoverflow.com/questions/10286514/why-is-googlemock-leaking-my-shared-ptr */
    ::testing::Mock::VerifyAndClearExpectations(session_manager_mock_.get());
}

TEST_F(SessionImplSmallLimitTests,
    RunWhenRequestWithBodyBiggerThanLimitWillFail)
{
    // Start the session, this will cause an async "loop"
    session_->run();

    /* Run the io context on another thread so I can call synchronous "client"
       calls on this thread */
    t_ = std::thread([this](){io_context_->run();});

    EXPECT_CALL(*session_manager_mock_, stop(session_));

    EXPECT_CALL(*request_handler_mock_, body_limit_reached(
        RequestChecker(endpoint_target, version, verb)))
            .WillOnce(::testing::Return(::testing::ByMove(response_413_)));

    boost::beast::http::write(stream_, request_);

    send_request_and_expect_response(
        false, boost::beast::http::status::payload_too_large);

    // Session stops itself
    finish();

    /* GMock keeps session_ alive as it is checked as a param to stop, this is
       required to make it think its not leaking it
       https://stackoverflow.com/questions/10286514/why-is-googlemock-leaking-my-shared-ptr */
    ::testing::Mock::VerifyAndClearExpectations(session_manager_mock_.get());
}

}
