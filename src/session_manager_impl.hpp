#ifndef HTTP_SESSION_MANAGER_IMPL_HPP
#define HTTP_SESSION_MANAGER_IMPL_HPP

#include <memory>
#include <unordered_set>

#include "session_manager.hpp"

namespace http
{

class session_manager_impl : public session_manager
{
public:
    void start(const std::shared_ptr<session>& session) override;

    void stop_all() override;

private:
    std::unordered_set<std::shared_ptr<session>> sessions_;
};

}

#endif
