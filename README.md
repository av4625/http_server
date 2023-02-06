[![Build | Unittests | Coverage](https://github.com/av4625/http_server/actions/workflows/build.yml/badge.svg)](https://github.com/av4625/http_server/actions/workflows/build.yml) [![CPPCheck](https://github.com/av4625/http_server/actions/workflows/cppcheck.yml/badge.svg)](https://github.com/av4625/http_server/actions/workflows/cppcheck.yml)

https://github.com/marketplace/actions/build-a-badge

# HTTP Server (WIP)
Single threaded http server that uses boost asio and is based on the single
threaded example from boost. The server uses a provided `io_context` so that it
can be used for other things at the same time.

# Third Party
## Boost
* 1.81.0 (Required for `message_generator` in `beast` and `url`)

Try and understand how to build header only using conan, maybe don't need the
`find_package` line at all and make a target that is the include directory.
Might need to add a src file that includes `boost/url/src.hpp` and at it to the
target or its own target.

## Gtest
* 1.12.1

# Boost Beast
https://www.boost.org/doc/libs/master/libs/beast/doc/html/beast/examples.html#beast.examples.servers
https://github.com/dfleury2/beauty
