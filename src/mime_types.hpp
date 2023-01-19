#ifndef HTTP_MIME_TYPES_HPP
#define HTTP_MIME_TYPES_HPP

#include <string>

namespace http
{

// Convert a file extension into a MIME type.
const std::string& extension_to_mime_type(const std::string& extension);

}

#endif
