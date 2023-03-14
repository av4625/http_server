#include <memory>

#include <boost/asio.hpp>

#include <http/server.hpp>
#include <http/server_factory.hpp>

int main()
{
    const std::shared_ptr<boost::asio::io_context> io_context(
        std::make_shared<boost::asio::io_context>());

    http::server_factory server_factory;
    const auto server(server_factory.create("80", io_context));

    server->serve_static("/path/to/html");

    server->on(
        "/endpoint",
        http::method::get,
        [](const http::request&, http::string_response& res)
        {
            res.set_status_code(http::status_code::ok);
            res.set_content("[\"thing1\", \"thing2\"]");
            res.add_header(http::field::content_type, "application/json");
            res.add_header(http::field::connection, "close");
            res.calculate_and_set_content_length();
        });

    server->start_server();

    io_context->run();
}
