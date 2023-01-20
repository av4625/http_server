#ifndef HTTP_CONNECTION_MANAGER_MOCK_HPP
#define HTTP_CONNECTION_MANAGER_MOCK_HPP

#include <gmock/gmock.h>

#include <connection_manager.hpp>

namespace http
{

class connection_manager_mock : public connection_manager
{
public:
    MOCK_METHOD(void, start, (const std::shared_ptr<connection>&), (override));

    MOCK_METHOD(void, stop, (const std::shared_ptr<connection>&), (override));

    MOCK_METHOD(void, stop_all, (), (override));
};

}

#endif
