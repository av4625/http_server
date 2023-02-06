#ifndef HTTP_SESSION_HPP
#define HTTP_SESSION_HPP

#include <boost/noncopyable.hpp>

namespace http
{

class session : private boost::noncopyable
{
public:
    virtual ~session() = default;

    virtual void run() = 0;

    virtual void stop() = 0;
};

}

#endif
