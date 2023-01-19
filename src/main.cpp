#include <memory>
#include <string>
#include <thread>

#include <boost/asio.hpp>
#include <boost/lexical_cast.hpp>

#include "server.hpp"
#include "server_factory.hpp"

int main()
{
    const std::shared_ptr<boost::asio::io_context> io_context(
        std::make_shared<boost::asio::io_context>());

    http::server_factory server_factory;
    const auto server(server_factory.create("80", io_context));

    server->serve_static("/path/to/html");

    server->on(
        "/timed_tracks.json",
        http::method::GET,
        [](const http::request&, http::response& res)
        {
            res.set_status_code(http::status_code::ok);
            res.set_content("[\"thing1\", \"thing2\"]");
            res.add_header(
                "Content-Length",
                boost::lexical_cast<std::string>(res.content_length()));
            res.add_header("Content-Type", "application/json");
            res.add_header("Connection", "close");
        });

    server->start_server();

    io_context->run();
}
