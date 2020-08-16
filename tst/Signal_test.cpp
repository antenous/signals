// Copyright (c) 2020 Antero Nousiainen

#include <signals/Signal.hpp>
#include <gtest/gtest.h>

using namespace testing;

namespace
{
    class SignalTest : public Test
    {
    protected:
        using Signal = signals::Signal<void()>;

        Signal signal;
    };

    TEST_F(SignalTest, IsDefaultConstructible)
    {
        EXPECT_TRUE(std::is_default_constructible_v<Signal>);
    }

    TEST_F(SignalTest, IsNoncopyable)
    {
        EXPECT_FALSE(std::is_copy_constructible_v<Signal>);
        EXPECT_FALSE(std::is_copy_assignable_v<Signal>);
    }

    TEST_F(SignalTest, IsNothrowMoveable)
    {
        EXPECT_TRUE(std::is_nothrow_move_constructible_v<Signal>);
        EXPECT_TRUE(std::is_nothrow_move_assignable_v<Signal>);
    }

    TEST_F(SignalTest, IsEmptyWhenNoSlotsAreConnected)
    {
        EXPECT_TRUE(signal.empty());
    }

    TEST_F(SignalTest, IsNotEmptyWhenSlotIsConnected)
    {
        signal.connect([]{}); // LCOV_EXCL_LINE
        EXPECT_FALSE(signal.empty());
    }

    TEST_F(SignalTest, IsEmptyWhenSlotsAreCleared)
    {
        signal.connect([]{}); // LCOV_EXCL_LINE
        signal.clear();
        EXPECT_TRUE(signal.empty());
    }

    TEST_F(SignalTest, DoNotCrashOnSignalWhenNoSlotsAreConnected)
    {
        signal();
    }

    TEST_F(SignalTest, InvokeConnectedSlotsOnSignal)
    {
        auto i = 1;
        signal.connect([&i]{ i *= 2; });
        signal.connect([&i]{ i += 3; });
        signal();
        EXPECT_EQ(5, i);
    }

    TEST_F(SignalTest, InvokeConnectedSlotsWithArgumentsOnSignal)
    {
        signals::Signal<void(int&)> signal;

        auto i = 1;
        signal.connect([](int & i){ i *= 2; });
        signal.connect([](int & i){ i += 3; });
        signal(i);

        EXPECT_EQ(5, i);
    }

    TEST_F(SignalTest, IsSafeForSelfMove)
    {
        auto i = 1;
        signal.connect([&i]{ i *= 2; });
        signal.connect([&i]{ i += 3; });
        signal = std::move(signal);
        signal();
        EXPECT_EQ(5, i);
    }
}
