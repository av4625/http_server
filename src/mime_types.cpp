#include "mime_types.hpp"

#include <unordered_map>

#include <boost/assign.hpp>

namespace http
{

namespace
{

const std::unordered_map<std::string, std::string> extension_mime_types{
    boost::assign::map_list_of
        ("gif", "image/gif")
        ("htm", "text/html")
        ("html", "text/html")
        ("php", "text/html")
        ("jpg", "image/jpeg")
        ("png", "image/png")
        ("css", "text/css")
        ("js", "text/javascript")
        ("txt", "text/plain")};

}

const std::string& extension_to_mime_type(const std::string& extension)
{
    if (extension_mime_types.contains(extension))
    {
        return extension_mime_types.at(extension);
    }

    return extension_mime_types.at("txt");
}

}
