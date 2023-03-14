#ifndef HTTP_REQUEST_HANDLER_IMPL_HPP
#define HTTP_REQUEST_HANDLER_IMPL_HPP

#include <memory>
#include <unordered_map>
#include <utility>

#include <boost/functional/hash.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/locks.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/shared_mutex.hpp>

#include "request_handler.hpp"
#include "response_fwd.hpp"

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
        std::function<void(const request&, file_response&)> callback) override;

    void add_request_handler(
        const std::string& uri,
        method method,
        std::function<void(const request&, string_response&)> callback) override;

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

    template <typename T>
    void add_handler(
        const std::string& uri,
        const method method,
        std::function<void(const request&, T&)> callback)
    {
        boost::unique_lock<boost::upgrade_mutex> lock(handlers_mutex_);

        handlers_.emplace(
            std::piecewise_construct,
            std::forward_as_tuple(std::make_pair(uri, method)),
            std::forward_as_tuple(std::make_shared<handler_impl<T> >(
                std::move(callback))));
    }

    class handler
    {
    public:
        virtual ~handler() = default;

        virtual boost::beast::http::message_generator operator()(
            const request& req, unsigned int version, bool keep_alive) const = 0;
    };

    template <typename T>
    class handler_impl : public handler
    {
    public:
        explicit handler_impl(std::function<void(const request&, T&)> f) :
            mutex_(),
            f_(std::move(f))
        {
        }

        boost::beast::http::message_generator operator()(
            const request& req,
            const unsigned int version,
            const bool keep_alive) const override
        {
            T res{version, keep_alive};
            boost::unique_lock<boost::mutex> lock(mutex_);
            f_(req, res);

            return res;
        }

    private:
        mutable boost::mutex mutex_;
        const std::function<void(const request&, T&)> f_;
    };

    // Map to hold custom request handlers <<uri, method>, handler>
    std::unordered_map<
        std::pair<std::string, method>,
        const std::shared_ptr<const handler>,
        boost::hash<std::pair<std::string, method> > > handlers_;
};

}

#endif
