#ifndef HTTP_REQUEST_HANDLER_IMPL_HPP
#define HTTP_REQUEST_HANDLER_IMPL_HPP

#include <unordered_map>
#include <utility>

#include <boost/functional/hash.hpp>

#include "request_handler.hpp"

namespace http
{

class request_handler_impl : public request_handler
{
public:
    request_handler_impl();

    void serve_from_directory(const std::string& doc_root) override;

    void add_request_handler(
        const std::string& uri,
        method method,
        std::function<void(const request&, response&)> callback) override;

    void handle_request(const request& req, response& res) const override;

private:
    // The directory containing the files to be served.
    std::string doc_root_;

    // struct pair_hash
    // {
    //     template <class T1, class T2>
    //     std::size_t operator()(const std::pair<T1, T2>& pair) const
    //     {
    //         return std::hash<T1>{}(pair.first) ^ std::hash<T2>{}(pair.second);
    //     }
    // };

    // Map to hold custom request handlers <<uri, method>, handler>
    std::unordered_map<
        std::pair<std::string, method>,
        std::function<void(const request&, response&)>,
        boost::hash<std::pair<std::string, method> > > handlers_;

    /* Perform URL-decoding on a string. Returns false if the encoding was
    invalid. */
    static bool url_decode(const std::string& in, std::string& out);
};

}

#endif
