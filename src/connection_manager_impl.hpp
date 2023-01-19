#ifndef HTTP_CONNECTION_MANAGER_IMPL_HPP
#define HTTP_CONNECTION_MANAGER_IMPL_HPP

#include <set>

#include "connection_manager.hpp"

namespace http
{

class connection_manager_impl : public connection_manager
{
public:
    void start(const std::shared_ptr<connection>& c) override;

    void stop(const std::shared_ptr<connection>& c) override;

    void stop_all() override;

private:
    // The managed connections.
    std::set<std::shared_ptr<connection> > connections_;
};

}

#endif
