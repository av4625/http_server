add_executable(http_server
    connection_impl.cpp
    connection_manager_impl.cpp
    main.cpp
    method.cpp
    mime_types.cpp
    request_handler_impl.cpp
    request_parser.cpp
    response.cpp
    server_factory.cpp
    server_impl.cpp
    status_code.cpp
    stock_responses.cpp)

target_link_libraries(http_server PUBLIC Boost::system)
