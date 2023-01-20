[![Build | Unittests | Coverage](https://github.com/av4625/http_server/actions/workflows/build.yml/badge.svg)](https://github.com/av4625/http_server/actions/workflows/build.yml) [![CPPCheck](https://github.com/av4625/http_server/actions/workflows/cppcheck.yml/badge.svg)](https://github.com/av4625/http_server/actions/workflows/cppcheck.yml)

https://github.com/marketplace/actions/build-a-badge

# HTTP Server (WIP)
Single threaded http server that uses boost asio and is based on the single
threaded example from boost. The server uses a provided `io_context` so that it
can be used for other things at the same time.
