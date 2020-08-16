// Copyright (c) 2020 Antero Nousiainen

#include <signals/Slot.hpp>
#include <gtest/gtest.h>

using namespace testing;

namespace
{
    class SlotTest : public Test
    {
    protected:
        using Slot = signals::Slot<int()>;
    };

    TEST_F(SlotTest, IsNotDefaultConstructible)
    {
        EXPECT_FALSE(std::is_default_constructible_v<Slot>);
    }

    TEST_F(SlotTest, IsNoncopyable)
    {
        EXPECT_FALSE(std::is_copy_constructible_v<Slot>);
        EXPECT_FALSE(std::is_copy_assignable_v<Slot>);
    }

    TEST_F(SlotTest, IsNothrowMoveConstructible)
    {
        EXPECT_TRUE(std::is_nothrow_move_constructible_v<Slot>);
        EXPECT_FALSE(std::is_move_assignable_v<Slot>);
    }

    TEST_F(SlotTest, CallableTypeIsStdFunction)
    {
        EXPECT_TRUE((std::is_same_v<std::function<int()>, Slot::Callable>));
    }

    TEST_F(SlotTest, ReturnResultOfCallableWhenInvoked)
    {
        const auto result = 42;
        const auto slot = Slot{[result]{ return result; }};
        EXPECT_EQ(result, std::invoke(slot));
    }
}
