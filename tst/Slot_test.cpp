// Copyright (c) 2020 Antero Nousiainen

#include <signals/Slot.hpp>
#include <gtest/gtest.h>

namespace
{
using namespace testing;

class SlotTest : public Test
{
protected:
    using Disconnectable = signals::Disconnectable;
    using Slot = signals::Slot<int()>;
};

TEST_F(SlotTest, IsDisconnectable)
{
    EXPECT_TRUE((std::is_base_of_v<Disconnectable, Slot>));
}

TEST_F(SlotTest, IsNotDefaultConstructible)
{
    EXPECT_FALSE(std::is_default_constructible_v<Slot>);
}

TEST_F(SlotTest, IsNoncopyable)
{
    EXPECT_FALSE(std::is_copy_constructible_v<Slot>);
    EXPECT_FALSE(std::is_copy_assignable_v<Slot>);
}

TEST_F(SlotTest, IsNonmoveable)
{
    EXPECT_FALSE(std::is_move_constructible_v<Slot>);
    EXPECT_FALSE(std::is_move_assignable_v<Slot>);
}

TEST_F(SlotTest, CallableTypeIsStdFunction)
{
    EXPECT_TRUE((std::is_same_v<std::function<int()>, Slot::Callable>));
}

TEST_F(SlotTest, ReturnType)
{
    EXPECT_TRUE((std::is_same_v<int, Slot::Result>));
    EXPECT_TRUE((std::is_same_v<void, signals::Slot<void(int)>::Result>));
}

TEST_F(SlotTest, ReturnResultOfCallableWhenInvoked)
{
    const auto result = 42;
    const auto fn = [=] {
        return result;
    };
    const auto slot = Slot{fn};
    EXPECT_EQ(result, std::invoke(slot));
}
} // namespace
