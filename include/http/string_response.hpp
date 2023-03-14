#ifndef HTTP_STRING_RESPONSE_HPP
#define HTTP_STRING_RESPONSE_HPP

#include <boost/beast/http/string_body.hpp>

#include "response_base.hpp"

namespace http
{

class string_response : public response_base<boost::beast::http::string_body>
{
public:
    string_response(unsigned int version, bool keep_alive);

    void set_content(const std::string& content);
};

}

#endif
