[![Build | Unittests | Coverage](https://github.com/av4625/http_server/actions/workflows/build.yml/badge.svg)](https://github.com/av4625/http_server/actions/workflows/build.yml) [![CPPCheck](https://github.com/av4625/http_server/actions/workflows/cppcheck.yml/badge.svg)](https://github.com/av4625/http_server/actions/workflows/cppcheck.yml)

https://github.com/marketplace/actions/build-a-badge

# HTTP Server (WIP)
Single threaded http server that uses boost asio and is based on the single
threaded example from boost. The server uses a provided `io_context` so that it
can be used for other things at the same time.

# Third Party
## Boost
* 1.86.0 (Required for `message_generator` in `beast`)
    * Only headers are required.

## Poco
* 1.13.3 (Required for URI parsing)
    * Only Foundation is required.

## Gtest
* 1.12.1

# Boost Beast
https://www.boost.org/doc/libs/master/libs/beast/doc/html/beast/examples.html#beast.examples.servers
https://github.com/dfleury2/beauty

# Can use your own boost
https://github.com/kenba/via-httplib
