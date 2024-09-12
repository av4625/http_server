# Tell find_package() to first search using Config mode before falling back to Module mode (for conan)
set(CMAKE_FIND_PACKAGE_PREFER_CONFIG TRUE)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_BINARY_DIR})
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_CURRENT_BINARY_DIR})

if(NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/conan.cmake")
    message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
    file(DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/0.18.1/conan.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/conan.cmake"
        TLS_VERIFY ON)
endif()

include(${CMAKE_CURRENT_BINARY_DIR}/conan.cmake)

conan_cmake_configure(
    REQUIRES boost/1.86.0 gtest/1.12.1 poco/1.13.3
    GENERATORS cmake_find_package
    OPTIONS
    boost:header_only=True
    boost:error_code_header_only=True
    boost:without_atomic=True
    boost:without_charconv=True
    boost:without_chrono=True
    boost:without_cobalt=True
    boost:without_container=True
    boost:without_context=True
    boost:without_contract=True
    boost:without_coroutine=True
    boost:without_date_time=True
    boost:without_exception=True
    boost:without_fiber=True
    boost:without_filesystem=True
    boost:without_graph=True
    boost:without_graph_parallel=True
    boost:without_iostreams=True
    boost:without_json=True
    boost:without_locale=True
    boost:without_log=True
    boost:without_math=True
    boost:without_mpi=True
    boost:without_nowide=True
    boost:without_program_options=True
    boost:without_python=True
    boost:without_random=True
    boost:without_regex=True
    boost:without_serialization=True
    boost:without_stacktrace=True
    boost:without_system=True
    boost:without_test=True
    boost:without_thread=True
    boost:without_timer=True
    boost:without_type_erasure=True
    boost:without_url=True
    boost:without_wave=True
    poco:enable_activerecord=False
    poco:enable_apacheconnector=False
    poco:enable_cppparser=False
    poco:enable_crypto=False
    poco:enable_data=False
    poco:enable_data_mysql=False
    poco:enable_data_odbc=False
    poco:enable_data_postgresql=False
    poco:enable_data_sqlite=False
    poco:enable_encodings=False
    poco:enable_fork=False
    poco:enable_json=False
    poco:enable_jwt=False
    poco:enable_mongodb=False
    poco:enable_net=False
    poco:enable_netssl=False
    poco:enable_pdf=False
    poco:enable_pagecompiler=False
    poco:enable_pagecompiler_file2page=False
    poco:enable_pocodoc=False
    poco:enable_redis=False
    poco:enable_sevenzip=False
    poco:enable_util=False
    poco:enable_xml=False
    poco:enable_zip=False
    poco:enable_activerecord=False
    poco:enable_activerecord_compiler=False
    poco:enable_prometheus=False
    poco:with_sql_parser=False)

conan_cmake_autodetect(settings)

conan_cmake_install(
    PATH_OR_REFERENCE .
    BUILD missing
    REMOTE conancenter
    SETTINGS ${settings})

find_package(
    Boost 1.86.0
    EXACT REQUIRED COMPONENTS
    headers
    REQUIRED GLOBAL)

find_package(GTest 1.12.1 EXACT REQUIRED COMPONENTS gtest gmock REQUIRED GLOBAL)

find_package(Poco 1.13.3 EXACT REQUIRED COMPONENTS Foundation REQUIRED GLOBAL)
