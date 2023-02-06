#ifndef HTTP_REQUEST_IMPL_HPP
#define HTTP_REQUEST_IMPL_HPP

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

    bool has_query_param(const std::string& key) const override;

    std::string get_query_param(const std::string& key) const override;

private:
    const boost::beast::http::request<boost::beast::http::string_body> request_;
    const std::string url_str_;
    const boost::urls::url_view url_;
};

}

#endif
