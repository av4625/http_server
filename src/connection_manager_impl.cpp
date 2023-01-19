#include "connection_manager_impl.hpp"

#include <algorithm>

#include <boost/bind/bind.hpp>

#include "connection.hpp"

namespace http
{

void connection_manager_impl::start(const std::shared_ptr<connection>& c)
{
    connections_.insert(c);
    c->start();
}

void connection_manager_impl::stop(const std::shared_ptr<connection>& c)
{
    connections_.erase(c);
    c->stop();
}

void connection_manager_impl::stop_all()
{
    std::for_each(connections_.begin(), connections_.end(),
        boost::bind(&connection::stop, boost::placeholders::_1));
    connections_.clear();
}

}
