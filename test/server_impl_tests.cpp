#include <gtest/gtest.h>

#include <thread>

#include "mocks/connection_manager_mock.hpp"
#include "mocks/request_handler_mock.hpp"

#include <request_impl.hpp>
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
        connection_manager_mock_(std::make_shared<connection_manager_mock>()),
        request_handler_mock_(std::make_shared<request_handler_mock>()),
        callback_check_(),
        callback_(),
        server_(std::make_shared<server_impl>(
            "0.0.0.0",
            "8080",
            io_context_,
            connection_manager_mock_,
            request_handler_mock_))
    {
    }

    const std::shared_ptr<boost::asio::io_context> io_context_;
    const std::shared_ptr<connection_manager_mock> connection_manager_mock_;
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
    const method m{method::GET};
    const std::function<void(const request_data&, response&)> callback{};

    EXPECT_CALL(*request_handler_mock_, add_request_handler(
        uri, m, ::testing::_)).WillOnce(::testing::Invoke(
            this, &ServerImplTests::save_callback));

    server_->on(
        uri,
        m,
        [this](const request&, response&){callback_check_.increment();});

    response res;
    request_data data;
    callback_(request_impl(data), res);
    EXPECT_EQ(1, callback_check_.i);
}

TEST_F(ServerImplTests, StartedServerCanBeStopped)
{
    EXPECT_CALL(*connection_manager_mock_, stop_all());

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
    EXPECT_CALL(*connection_manager_mock_, stop_all());
    server_->end_server();
}

}
