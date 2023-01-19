#ifndef HTTP_CONNECTION_MANAGER_HPP
#define HTTP_CONNECTION_MANAGER_HPP

#include <memory>

#include <boost/noncopyable.hpp>

#include "connection_fwd.hpp"

namespace http
{

/* Manages open connections so that they may be cleanly stopped when the server
   needs to shut down. */
class connection_manager : private boost::noncopyable
{
public:
    virtual ~connection_manager() = default;

    // Add the specified connection to the manager and start it.
    virtual void start(const std::shared_ptr<connection>& c) = 0;

    // Stop the specified connection.
    virtual void stop(const std::shared_ptr<connection>& c) = 0;

    // Stop all connections.
    virtual void stop_all() = 0;
};

}

#endif
