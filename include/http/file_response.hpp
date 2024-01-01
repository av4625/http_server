#ifndef HTTP_FILE_RESPONSE_HPP
#define HTTP_FILE_RESPONSE_HPP

#include <boost/beast/http/file_body.hpp>

#include "response_base.hpp"

namespace http
{

class file_response : public response_base<boost::beast::http::file_body>
{
public:
    file_response(unsigned int version, bool keep_alive);

    operator boost::beast::http::message_generator() override;

    /**
     * @brief Set the content for the response.
     *
     * @param content[in] The content as a string.
    */
    void set_content(const std::string& path);

private:
    const unsigned int version_;
    const bool keep_alive_;
};

}

#endif
