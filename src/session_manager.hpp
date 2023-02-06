#ifndef HTTP_SESSION_MANAGER_HPP
#define HTTP_SESSION_MANAGER_HPP

#include <boost/noncopyable.hpp>

#include "session_fwd.hpp"

namespace http
{

/**
 * Manager to manage sessions so that we can manaually stop them.
 * This manager is not thread safe
*/
class session_manager : private boost::noncopyable
{
public:
    virtual ~session_manager() = default;

    virtual void start(const std::shared_ptr<session>& session) = 0;

    virtual void stop(const std::shared_ptr<session>& session) = 0;

    virtual void stop_all() = 0;
};

}

#endif
