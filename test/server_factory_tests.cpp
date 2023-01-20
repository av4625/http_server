#include <gtest/gtest.h>

#include <server.hpp>
#include <server_factory.hpp>

namespace http
{

TEST(ServerFactoryTests, CreateWillReturnAServer)
{
    const server_factory factory;

    const auto server{factory.create(
        "8080", std::make_shared<boost::asio::io_context>())};

    EXPECT_TRUE(server);
}

}
