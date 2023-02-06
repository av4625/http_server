add_library(unittests STATIC main.cpp)

target_link_libraries(unittests PUBLIC GTest::gtest)

add_executable(http_server_tests
    mime_types_tests.cpp
    request_handler_impl_tests.cpp
    request_impl_tests.cpp
    response_tests.cpp
    server_factory_tests.cpp
    server_impl_tests.cpp
    session_impl_tests.cpp
    session_manager_impl_tests.cpp
    stock_responses_tests.cpp)

target_link_libraries(
    http_server_tests PRIVATE GTest::gmock http_server unittests)

add_test(NAME http_server_tests COMMAND http_server_tests)

if(COVERAGE AND CMAKE_BUILD_TYPE STREQUAL "Debug")

    if(NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/CodeCoverage.cmake")
        message(STATUS "Downloading CodeCoverage.cmake from https://github.com/bilke/cmake-modules/blob/master/CodeCoverage.cmake")
        file(DOWNLOAD "https://raw.githubusercontent.com/bilke/cmake-modules/master/CodeCoverage.cmake"
            "${CMAKE_CURRENT_BINARY_DIR}/CodeCoverage.cmake"
            TLS_VERIFY ON)
    endif()

    include(${CMAKE_CURRENT_BINARY_DIR}/CodeCoverage.cmake)

    APPEND_COVERAGE_COMPILER_FLAGS()

    SETUP_TARGET_FOR_COVERAGE_LCOV(
        NAME coverage
        LCOV_ARGS "--no-external"
        EXECUTABLE ctest -j
        BASE_DIRECTORY "../src"
        DEPENDENCIES http_server_tests)

endif()
# https://stackoverflow.com/questions/52255589/code-coverage-analysis-using-codecoverage-cmake-with-multiple-targets
# setup_target_for_coverage_gcovr_html(NAME coverage EXECUTABLE ctest --schedule-random -j 4 --test-dir test EXCLUDE "/usr/*" "/mnt/v/r3dapple.github.io/encryption/mhcl/test/*" "/mnt/v/r3dapple.github.io/encryption/mhcl/src/main.cpp" DEPENDENCIES test_biginteger_constructor test_biginteger_add test_biginteger_multiply test_biginteger_divide)
