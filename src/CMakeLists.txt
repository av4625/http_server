add_library(http_server
    file_response.cpp
    mime_types.cpp
    request_handler_impl.cpp
    request_impl.cpp
    server_factory.cpp
    server_impl.cpp
    session_impl.cpp
    session_manager_impl.cpp
    stock_responses.cpp
    string_response.cpp)

target_link_libraries(http_server
    PUBLIC
        Boost::boost
    PRIVATE
        Boost::url Boost::thread)

target_include_directories(
    http_server PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include)
