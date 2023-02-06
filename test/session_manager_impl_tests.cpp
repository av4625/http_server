#include <gtest/gtest.h>

#include "mocks/session_mock.hpp"
#include "mocks/request_handler_mock.hpp"

#include <session_manager_impl.hpp>

namespace http
{

namespace
{

class SessionManagerImplTests : public ::testing::Test
{
protected:
    SessionManagerImplTests() :
        session_mock_(std::make_shared<session_mock>()),
        session_manager_()
    {
    }

    const std::shared_ptr<session_mock> session_mock_;
    session_manager_impl session_manager_;
};

}

TEST_F(SessionManagerImplTests, StartWillSaveAndRunSession)
{
    EXPECT_CALL(*session_mock_, run());
    session_manager_.start(session_mock_);
}

TEST_F(SessionManagerImplTests, StopAllWillStopAllSessionsAndRemoveThem)
{
    EXPECT_CALL(*session_mock_, run());
    EXPECT_CALL(*session_mock_, stop());
    session_manager_.start(session_mock_);

    const std::shared_ptr<session_mock> session_mock(
        std::make_shared<session_mock>());
    EXPECT_CALL(*session_mock, run());
    EXPECT_CALL(*session_mock, stop());
    session_manager_.start(session_mock);

    session_manager_.stop_all();

    // Call again to prove they are removed
    session_manager_.stop_all();
}

}
