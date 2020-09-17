// Copyright (c) 2020 Antero Nousiainen

#include <signals/Connection.hpp>
#include <signals/Signal.hpp>
#include <gtest/gtest.h>

using namespace signals;
using namespace testing;

namespace
{
    class ConnectionTest : public Test
    {
    protected:
        Connection connection;
        Signal<void()> signal;
    };

    TEST_F(ConnectionTest, IsNothrowDefaultConstructible)
    {
        EXPECT_TRUE(std::is_nothrow_default_constructible_v<Connection>);
    }

    TEST_F(ConnectionTest, IsCopyable)
    {
        EXPECT_TRUE(std::is_copy_constructible_v<Connection>);
        EXPECT_TRUE(std::is_copy_assignable_v<Connection>);
    }

    TEST_F(ConnectionTest, IsNothrowMoveable)
    {
        EXPECT_TRUE(std::is_nothrow_move_constructible_v<Connection>);
        EXPECT_TRUE(std::is_nothrow_move_assignable_v<Connection>);
    }

    TEST_F(ConnectionTest, HasVirtualDestructor)
    {
        EXPECT_TRUE(std::has_virtual_destructor_v<Connection>);
    }

    TEST_F(ConnectionTest, IsNotConnectedWhenDefaultConstructed)
    {
        EXPECT_FALSE(connection.connected());
    }

    TEST_F(ConnectionTest, IsConnectedWhenSlotIsConnected)
    {
        const auto connection = signal.connect([]{}); // LCOV_EXCL_LINE
        EXPECT_TRUE(connection.connected());
    }

    TEST_F(ConnectionTest, IsNotConnectedWhenSlotIsDisconnected)
    {
        auto connection = signal.connect([]{}); // LCOV_EXCL_LINE
        connection.disconnect();
        EXPECT_FALSE(connection.connected());
    }

    TEST_F(ConnectionTest, DoNotCrashWhenDisconnectingAlreadyDisconnectedSlot)
    {
        connection.disconnect();
    }

    TEST_F(ConnectionTest, DisconnectOriginalWhenCopyIsDisconnected)
    {
        const auto original = signal.connect([]{}); // LCOV_EXCL_LINE
        auto copy{original};
        EXPECT_TRUE(original.connected() && copy.connected());

        copy.disconnect();
        EXPECT_FALSE(original.connected() && copy.connected());
    }

    TEST_F(ConnectionTest, DisconnectOriginalWhenMoved)
    {
        auto original = signal.connect([]{}); // LCOV_EXCL_LINE
        const auto copy{std::move(original)};
        EXPECT_TRUE(!original.connected() && copy.connected());
    }

    TEST_F(ConnectionTest, IsSafeForSelfMove)
    {
        auto connection = signal.connect([]{}); // LCOV_EXCL_LINE
        connection = std::move(connection);
        EXPECT_TRUE(connection.connected());
    }
}
