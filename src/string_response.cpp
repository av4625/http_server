#include <http/string_response.hpp>

namespace http
{

string_response::string_response(
    const unsigned int version, const bool keep_alive) :
        response_base(version, keep_alive)
{
}

void string_response::set_content(const std::string& content)
{
    response_.body() = content;
}

}
