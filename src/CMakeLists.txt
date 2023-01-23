add_library(http_server
    connection_impl.cpp
    connection_manager_impl.cpp
    method.cpp
    mime_types.cpp
    request_handler_impl.cpp
    request_impl.cpp
    request_parser.cpp
    response.cpp
    server_factory.cpp
    server_impl.cpp
    status_code.cpp
    stock_responses.cpp)

target_link_libraries(http_server PUBLIC Boost::system)

add_executable(http_server_test main.cpp)
target_link_libraries(http_server_test PUBLIC http_server)
