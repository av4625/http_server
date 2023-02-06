#ifndef HTTP_SESSION_MANAGER_MOCK_HPP
#define HTTP_SESSION_MANAGER_MOCK_HPP

#include <gmock/gmock.h>

#include <session_manager.hpp>

namespace http
{

class session_manager_mock : public session_manager
{
public:
    MOCK_METHOD(void, start, (const std::shared_ptr<session>&), (override));

    MOCK_METHOD(void, stop_all, (), (override));
};

}

#endif
