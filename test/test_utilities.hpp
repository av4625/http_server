#ifndef HTTP_TEST_UTILITIES_HPP
#define HTTP_TEST_UTILITIES_HPP

#include <string>
#include <vector>

#include <boost/asio/buffers_iterator.hpp>
#include <boost/beast/http/message_generator.hpp>

namespace http
{

bool needle_in_haystack(const std::string& haystack, const std::string& needle);

struct has_substrs
{
public:
    explicit has_substrs(std::vector<std::string>&& needles);

    bool operator()(const std::string& haystack);

private:
    std::vector<std::string> needles;
    unsigned int needles_found_;
};

std::vector<std::string> expect_buffers_has_substrs(
    boost::beast::http::message_generator&& msg,
    has_substrs&& has_substrs);

}

#endif
