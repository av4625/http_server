#include <gtest/gtest.h>

#include <thread>

#include "mocks/session_manager_mock.hpp"
#include "mocks/request_handler_mock.hpp"

#include <request_impl.hpp>
#include <response.hpp>
#include <server_impl.hpp>

namespace http
{

namespace
{

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
        session_manager_mock_(std::make_unique<session_manager_mock>()),
        session_manager_mock_ptr_(session_manager_mock_.get()),
        request_handler_mock_(std::make_shared<request_handler_mock>()),
        callback_check_(),
        callback_(),
        server_(std::make_shared<server_impl>(
            "0.0.0.0",
            "8080",
            io_context_,
            std::move(session_manager_mock_),
            request_handler_mock_))
    {
    }

    const std::shared_ptr<boost::asio::io_context> io_context_;
    std::unique_ptr<session_manager_mock> session_manager_mock_;
    session_manager_mock* const session_manager_mock_ptr_;
    const std::shared_ptr<request_handler_mock> request_handler_mock_;
    callback_check callback_check_;
    std::function<void(const request&, response&)> callback_;
    const std::shared_ptr<server> server_;

public:
    void save_callback(
        const std::string&,
        const method,
        std::function<void(const request&, response&)> callback)
    {
        callback_ = std::move(callback);
    }
};

}

TEST_F(ServerImplTests, ServeStaticWillPassPathToRequestHandler)
{
    const std::string path{"/path/to/html"};
    EXPECT_CALL(*request_handler_mock_, serve_from_directory(path));
    server_->serve_static(path);
}

TEST_F(ServerImplTests, OnWillAddRequestHandler)
{
    const std::string uri{"/endpoint.txt"};
    const method m{method::get};

    EXPECT_CALL(*request_handler_mock_, add_request_handler(
        uri, m, ::testing::_)).WillOnce(::testing::Invoke(
            this, &ServerImplTests::save_callback));

    server_->on(
        uri,
        m,
        [this](const request&, response&){callback_check_.increment();});

    response res(11);
    callback_(
        request_impl(
            boost::beast::http::request<boost::beast::http::string_body>(
                boost::beast::http::verb::get, "/", 11)),
        res);
    EXPECT_EQ(1, callback_check_.i);
}

TEST_F(ServerImplTests, StartedServerCanBeStopped)
{
    EXPECT_CALL(*session_manager_mock_ptr_, stop_all());

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
    EXPECT_CALL(*session_manager_mock_ptr_, stop_all());
    server_->end_server();
}

}
