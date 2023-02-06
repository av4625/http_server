#include <algorithm>
#include <functional>

#include "session.hpp"
#include "session_manager_impl.hpp"

namespace http
{

void session_manager_impl::start(const std::shared_ptr<session>& session)
{
    sessions_.insert(session);
    session->run();
}

void session_manager_impl::stop_all()
{
    std::for_each(
        sessions_.begin(),
        sessions_.end(),
        [](const std::shared_ptr<session>& s){s->stop();});
    sessions_.clear();
}

}
