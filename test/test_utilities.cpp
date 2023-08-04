#include "test_utilities.hpp"

#include <algorithm>

#include <gtest/gtest.h>

namespace http
{

bool needle_in_haystack(const std::string& haystack, const std::string& needle)
{
    if (haystack.find(needle) != std::string::npos)
    {
        return true;
    }

    return false;
}

has_substrs::has_substrs(std::vector<std::string>&& needles) :
    needles_(std::move(needles))
{
}

bool has_substrs::operator()(const std::string& haystack)
{
    needles_.erase(
        std::remove_if(
            needles_.begin(),
            needles_.end(),
            std::bind(needle_in_haystack, haystack, std::placeholders::_1)),
        needles_.end());

    if (needles_.size() == 0)
    {
        return true;
    }

    return false;
}

std::vector<std::string> expect_buffers_has_substrs(
    boost::beast::http::message_generator&& msg,
    has_substrs&& has_substrs)
{
    boost::system::error_code ec;
    const auto buffer{msg.prepare(ec)};

    std::vector<std::string> response_text;
    response_text.resize(buffer.size(), "");

    for (unsigned int i = 0; i < buffer.size(); ++i)
    {
        std::copy(
            boost::asio::buffers_begin(buffer[i]),
            boost::asio::buffers_begin(buffer[i]) + buffer[i].size(),
            std::back_inserter(response_text[i]));
    }

    const auto it{std::find_if(
        response_text.begin(),
        response_text.end(),
        has_substrs)};

    EXPECT_NE(response_text.end(), it);

    return response_text;
}

}
