#ifndef HTTP_REQUEST_IMPL_HPP
#define HTTP_REQUEST_IMPL_HPP

#include "request.hpp"
#include "request_data.hpp"

namespace http
{

// A class to provide unmodifiable access to the request
class request_impl : public request
{
public:
    explicit request_impl(const request_data& data);

    bool has_query_param(const std::string& key) const override;

    const std::string& get_query_param(const std::string& key) const override;

private:
    /* Stored as a ref so the data must be kept alive outside this class.
       Might be better to use a shared pointer for safety, as performance and
       memory hit shouldn't be big.
       The request_data is currently kept alive by a connection  */
    const request_data& data_;
};

}

#endif
