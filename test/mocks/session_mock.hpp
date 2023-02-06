#ifndef HTTP_SESSION_MOCK_HPP
#define HTTP_SESSION_MOCK_HPP

#include <gmock/gmock.h>

#include <session.hpp>

namespace http
{

class session_mock : public session
{
public:
    MOCK_METHOD(void, run, (), (override));

    MOCK_METHOD(void, stop, (), (override));
};

}

#endif
