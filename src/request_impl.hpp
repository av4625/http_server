#ifndef HTTP_REQUEST_IMPL_HPP
#define HTTP_REQUEST_IMPL_HPP

#include <sstream>

#include <boost/beast/http/message.hpp>
#include <boost/beast/http/string_body.hpp>
#include <boost/url/url_view.hpp>

#include <http/request.hpp>

namespace http
{

class request_impl : public request
{
public:
    explicit request_impl(boost::beast::http::request<
        boost::beast::http::string_body>&& request);

    method get_method() const override;

    std::string get_endpoint() const override;

    bool has_query_param(const std::string& key) const override;

    std::string get_query_param(const std::string& key) const override;

    bool has_header(const std::string& name) const override;

    bool has_header(field name) const override;

    std::string get_header_value(const std::string& name) const override;

    std::string get_header_value(field name) const override;

    const std::string& body() const override;

private:
    const boost::beast::http::request<boost::beast::http::string_body> request_;
    const std::string url_str_;
    const boost::urls::url_view url_;

    template <typename T>
    bool has_header_(const T& name) const
    {
        return request_.find(name) != request_.end();
    }

    template <typename T>
    std::string get_header_value_(const T& name) const
    {
        const auto header{request_.find(name)};

        if (header != request_.end())
        {
            return header->value();
        }

        std::stringstream ss;
        ss << name << " does not exist as a header for this request";

        throw std::invalid_argument(ss.str());
    }
};

}

#endif
