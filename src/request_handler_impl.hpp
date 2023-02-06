#ifndef HTTP_REQUEST_HANDLER_IMPL_HPP
#define HTTP_REQUEST_HANDLER_IMPL_HPP

#include <unordered_map>
#include <utility>

#include <boost/functional/hash.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/locks.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/shared_mutex.hpp>

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

    boost::beast::http::message_generator handle_request(
        boost::beast::http::request<
            boost::beast::http::string_body>&& request) const override;

    void reset() override;

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

    mutable boost::upgrade_mutex handlers_mutex_;

    struct handler
    {
    public:
        explicit handler(std::function<void(const request&, response&)> f) :
            mutex_(),
            f_(std::move(f))
        {
        }

        void operator()(const request& req, response& res) const
        {
            boost::unique_lock<boost::mutex> lock(mutex_);
            f_(req, res);
        }

    private:
        mutable boost::mutex mutex_;
        const std::function<void(const request&, response&)> f_;
    };

    // Map to hold custom request handlers <<uri, method>, handler>
    std::unordered_map<
        std::pair<std::string, method>,
        handler,
        boost::hash<std::pair<std::string, method> > > handlers_;
};

}

#endif
