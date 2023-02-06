add_library(http_server
    mime_types.cpp
    request_handler_impl.cpp
    request_impl.cpp
    response.cpp
    server_factory.cpp
    server_impl.cpp
    session_impl.cpp
    session_manager_impl.cpp
    stock_responses.cpp)

target_link_libraries(http_server PUBLIC Boost::boost Boost::url)

add_executable(http_server_test main.cpp)
target_link_libraries(http_server_test PUBLIC http_server)
