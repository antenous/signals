// Copyright (c) 2020 Antero Nousiainen

#include <signals/Connection.hpp>
#include <gtest/gtest.h>

using namespace signals;
using namespace testing;

namespace
{
    class Slot : public Disconnectable
    {
    public:
        [[nodiscard]] bool connected() const override
        {
            return isConnected;
        }

        void disconnect() override
        {
            isConnected = false;
        }

    private:
        bool isConnected = true;
    };

    class ConnectionTest : public Test
    {
    protected:
        Connection::Disconnectable slot = std::make_shared<Slot>();
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

    TEST_F(ConnectionTest, IsNotConnectedByDefault)
    {
        EXPECT_FALSE(Connection{}.connected());
    }

    TEST_F(ConnectionTest, IsConnectedWhenSlotIsConnected)
    {
        const auto connection = Connection{slot};

        EXPECT_TRUE(connection.connected());
        EXPECT_TRUE(slot->connected());
    }

    TEST_F(ConnectionTest, DisconnectSlotWhenDisconnected)
    {
        auto connection = Connection{slot};

        connection.disconnect();

        EXPECT_FALSE(connection.connected());
        EXPECT_FALSE(slot->connected());
    }

    TEST_F(ConnectionTest, IsDisconnectedWhenSlotIsReset)
    {
        const auto connection = Connection{slot};

        slot.reset();

        EXPECT_FALSE(connection.connected());
        EXPECT_FALSE(slot);
    }

    TEST_F(ConnectionTest, DoNothingWhenResetSlotIsDisconnected)
    {
        // Disconnecting an already disconnected connection can lead to a crash if the
        // status of the slot is not checked before disconnecting it. Hence this test.
        auto connection = Connection{slot};
        slot.reset();

        connection.disconnect();
    }

    TEST_F(ConnectionTest, DoNotDisconnectWhenCopied)
    {
        const auto source = Connection{slot};

        const auto target = source;

        EXPECT_TRUE(source.connected());
        EXPECT_TRUE(target.connected());
        EXPECT_TRUE(slot->connected());
    }

    TEST_F(ConnectionTest, DisconnectAllWhenOneCopyIsDisconnected)
    {
        const auto source = Connection{slot};
        auto target = source;

        target.disconnect();

        EXPECT_FALSE(source.connected());
        EXPECT_FALSE(target.connected());
        EXPECT_FALSE(slot->connected());
    }

    TEST_F(ConnectionTest, DisconnectSourceWhenMoved)
    {
        auto source = Connection{slot};

        const auto target = std::move(source);

        EXPECT_FALSE(source.connected());
        EXPECT_TRUE(target.connected());
        EXPECT_TRUE(slot->connected());
    }

    TEST_F(ConnectionTest, DisconnectSourceWhenMoveAssigned)
    {
        auto source = Connection{slot};
        auto target = Connection{};

        target = std::move(source);

        EXPECT_FALSE(source.connected());
        EXPECT_TRUE(target.connected());
        EXPECT_TRUE(slot->connected());
    }

    TEST_F(ConnectionTest, DoNotDisconnectTargetSlotIsWhenMoveAssigned)
    {
        auto source = Connection{};
        auto target = Connection{slot};

        target = std::move(source);

        EXPECT_FALSE(source.connected());
        EXPECT_FALSE(target.connected());
        EXPECT_TRUE(slot->connected());
    }

    TEST_F(ConnectionTest, IsSelfMoveSafe)
    {
        auto connection = Connection{slot};
        const auto self = &connection;

        connection = std::move(*self);

        EXPECT_TRUE(connection.connected());
        EXPECT_TRUE(slot->connected());
    }
}
