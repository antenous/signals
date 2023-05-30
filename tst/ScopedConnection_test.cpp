// Copyright (c) 2020 Antero Nousiainen

#include <signals/ScopedConnection.hpp>
#include <gtest/gtest.h>

namespace signals
{
namespace
{
using namespace testing;

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

class ScopedConnectionTest : public Test
{
protected:
    Connection::Disconnectable slot = std::make_shared<Slot>();
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
    auto connection = Connection{slot};

    ScopedConnection copied = connection;
    EXPECT_TRUE(copied.connected());

    ScopedConnection moved = std::move(connection);
    EXPECT_TRUE(moved.connected());
}

TEST_F(ScopedConnectionTest, DoNotDisconnectSourceConnectionWhenCopied)
{
    auto source = Connection{slot};

    const auto target = ScopedConnection{source};

    EXPECT_TRUE(source.connected());
    EXPECT_TRUE(target.connected());
}

TEST_F(ScopedConnectionTest, DisconnectSourceConnectionWhenMoved)
{
    auto source = Connection{slot};

    const auto target = ScopedConnection{std::move(source)};

    EXPECT_FALSE(source.connected());
    EXPECT_TRUE(target.connected());
    EXPECT_TRUE(slot->connected());
}

TEST_F(ScopedConnectionTest, DisconnectSourceWhenMoved)
{
    ScopedConnection source = Connection{slot};

    const auto target = std::move(source);

    EXPECT_FALSE(source.connected());
    EXPECT_TRUE(target.connected());
    EXPECT_TRUE(slot->connected());
}

TEST_F(ScopedConnectionTest, DisconnectWhenConnectionCopyAssigned)
{
    const auto connection = Connection{slot};
    auto target = ScopedConnection{connection};

    const auto source = Connection{};
    target = source;

    EXPECT_FALSE(connection.connected());
}

TEST_F(ScopedConnectionTest, DoNotDisconnectSourceConnectionWhenConnectionCopyAssigned)
{
    auto target = ScopedConnection{};

    const auto source = Connection{slot};
    target = source;

    EXPECT_TRUE(source.connected());
    EXPECT_TRUE(target.connected());
}

TEST_F(ScopedConnectionTest, DisconnectWhenMoveAssigned)
{
    const auto connection = Connection{slot};
    auto target = ScopedConnection{connection};

    auto source = ScopedConnection{};
    target = std::move(source);

    EXPECT_FALSE(connection.connected());
    EXPECT_FALSE(source.connected());
    EXPECT_FALSE(target.connected());
}

TEST_F(ScopedConnectionTest, DisconnectSourceWhenMoveAssigned)
{
    auto target = ScopedConnection{};

    const auto connection = Connection{slot};
    auto source = ScopedConnection{connection};
    target = std::move(source);

    EXPECT_TRUE(connection.connected());
    EXPECT_FALSE(source.connected());
    EXPECT_TRUE(target.connected());
}

TEST_F(ScopedConnectionTest, DisconnectWhenConnectionMoveAssigned)
{
    const auto connection = Connection{slot};
    auto target = ScopedConnection{connection};

    target = Connection{};

    EXPECT_FALSE(connection.connected());
}

TEST_F(ScopedConnectionTest, DisconnectSourceConnectionWhenConnectionMoveAssigned)
{
    auto target = ScopedConnection{};

    auto source = Connection{slot};
    target = std::move(source);

    EXPECT_FALSE(source.connected());
    EXPECT_TRUE(target.connected());
    EXPECT_TRUE(slot->connected());
}

TEST_F(ScopedConnectionTest, DisconnectWhenOutOfScope)
{
    const auto connection = Connection{slot};
    {
        const auto scopedConnection = ScopedConnection{connection};
        EXPECT_TRUE(connection.connected());
    }
    EXPECT_FALSE(connection.connected());
}

TEST_F(ScopedConnectionTest, DoNotDisconnectWhenReleasedBeforeOutOfScope)
{
    auto connection = Connection{};
    {
        ScopedConnection scopedConnection = Connection{slot};
        connection = scopedConnection.release();
    }
    EXPECT_TRUE(connection.connected());
}

TEST_F(ScopedConnectionTest, IsSelfMoveSafe)
{
    ScopedConnection scopedConnection = Connection{slot};
    const auto self = &scopedConnection;

    scopedConnection = std::move(*self);

    EXPECT_TRUE(scopedConnection.connected());
}
} // namespace
} // namespace signals
