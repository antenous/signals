// Copyright (c) 2020 Antero Nousiainen

#include <signals/ScopedConnection.hpp>
#include <signals/Signal.hpp>
#include <gtest/gtest.h>

using namespace signals;
using namespace testing;

namespace
{
    class ScopedConnectionTest : public Test
    {
    protected:
        Signal<void()> signal;
    };

    TEST_F(ScopedConnectionTest, IsDefaultConstructible)
    {
        EXPECT_TRUE(std::is_default_constructible_v<ScopedConnection>);
    }

    TEST_F(ScopedConnectionTest, IsNoncopyable)
    {
        EXPECT_FALSE(std::is_copy_constructible_v<ScopedConnection>);
        EXPECT_FALSE(std::is_copy_assignable_v<ScopedConnection>);
    }

    TEST_F(ScopedConnectionTest, IsNothrowMoveable)
    {
        EXPECT_TRUE(std::is_nothrow_move_constructible_v<ScopedConnection>);
        EXPECT_TRUE(std::is_nothrow_move_assignable_v<ScopedConnection>);
    }

    TEST_F(ScopedConnectionTest, IsDerivedFromConnection)
    {
        EXPECT_TRUE((std::is_base_of_v<Connection, ScopedConnection>));
    }

    TEST_F(ScopedConnectionTest, IsImplicitlyConstructibleFromConnection)
    {
        ScopedConnection moved = signal.connect([]{}); // LCOV_EXCL_LINE
        EXPECT_TRUE(moved.connected());

        auto connection = signal.connect([]{}); // LCOV_EXCL_LINE
        ScopedConnection copied = connection;
        EXPECT_TRUE(copied.connected());
    }

    TEST_F(ScopedConnectionTest, DisconnectSlotWhenConnectionGoesOutOfScope)
    {
        const auto connection = signal.connect([]{}); // LCOV_EXCL_LINE
        {
            const auto scopedConnection = ScopedConnection{connection};
            EXPECT_TRUE(connection.connected());
        }
        EXPECT_FALSE(connection.connected());
    }

    TEST_F(ScopedConnectionTest, ObtainSlotOwnershipFromMovedConnection)
    {
        auto connection = signal.connect([]{}); // LCOV_EXCL_LINE
        const auto scopedConnection = ScopedConnection{std::move(connection)};
        EXPECT_TRUE(scopedConnection.connected() && !connection.connected());
    }

    TEST_F(ScopedConnectionTest, DisconnectSlotWhenNewIsMoved)
    {
        const auto connection(signal.connect([]{})); // LCOV_EXCL_LINE
        auto scopedConnection = ScopedConnection{connection};
        EXPECT_TRUE(connection.connected());

        scopedConnection = signal.connect([]{}); // LCOV_EXCL_LINE
        EXPECT_FALSE(connection.connected());
        EXPECT_TRUE(scopedConnection.connected());
    }

    TEST_F(ScopedConnectionTest, DisconnectSlotWhenNewIsCopied)
    {
        const auto connection(signal.connect([]{})); // LCOV_EXCL_LINE
        auto scopedConnection = ScopedConnection{connection};
        EXPECT_TRUE(connection.connected());

        const auto newConnection = signal.connect([]{}); // LCOV_EXCL_LINE
        scopedConnection = newConnection;
        EXPECT_FALSE(connection.connected());
        EXPECT_TRUE(scopedConnection.connected());
    }

    TEST_F(ScopedConnectionTest, DoNotDisconnectReleasedSlotWhenOutOfScope)
    {
        Connection connection;
        {
            auto scopedConnection = ScopedConnection{signal.connect([]{})}; // LCOV_EXCL_LINE
            connection = scopedConnection.release();
        }
        EXPECT_TRUE(connection.connected());
    }

    TEST_F(ScopedConnectionTest, IsSafeForSelfMove)
    {
        auto scopedConnection = ScopedConnection{signal.connect([]{})}; // LCOV_EXCL_LINE
        scopedConnection = std::move(scopedConnection);
        EXPECT_TRUE(scopedConnection.connected());
    }
}
