#ifndef HTTP_STOCK_RESPONSE_HPP
#define HTTP_STOCK_RESPONSE_HPP

#include <boost/beast/http/message.hpp>
#include <boost/beast/http/string_body.hpp>

#include "status_code.hpp"

namespace http
{

boost::beast::http::response<boost::beast::http::string_body> stock_reply(
    status_code status, bool keep_alive, unsigned int version);

}

#endif
