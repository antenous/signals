// Copyright (c) 2020 Antero Nousiainen

#include <signals/Signal.hpp>
#include <gtest/gtest.h>

using namespace testing;

namespace
{
    std::weak_ptr<std::size_t> bytesAllocated;

    class SignalTest : public Test
    {
    protected:
        using Signal = signals::Signal<void()>;

        void SetUp() override
        {
            ::bytesAllocated = bytesAllocated = std::make_shared<std::size_t>(0);
        }

        auto measureSizeofSlot(typename Signal::Slot::Callable callable) const
        {
            const auto bytesBefore = *bytesAllocated;
            auto slot = std::make_shared<Signal::Slot>(std::move(callable));
            return *bytesAllocated - bytesBefore;
        }

        std::shared_ptr<std::size_t> bytesAllocated;
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
        signals::Signal<void(int&)> signalWithParameters;

        auto i = 1;
        signalWithParameters.connect([](int & i){ i *= 2; });
        signalWithParameters.connect([](int & i){ i += 3; });
        signalWithParameters(i);

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

    TEST_F(SignalTest, DoNotInvokeDisconnectedSlotOnSignal)
    {
        auto i = 1;
        auto connection = signal.connect([&i]{ i *= 2; }); // LCOV_EXCL_LINE
        signal.connect([&i]{ i += 3; });
        connection.disconnect();
        signal();
        EXPECT_EQ(4, i);
    }

    TEST_F(SignalTest, IsEmptyWhenSlotsAreDisconnected)
    {
        auto connection = signal.connect([]{}); // LCOV_EXCL_LINE
        connection.disconnect();
        EXPECT_TRUE(signal.empty());
    }

    TEST_F(SignalTest, DoNotCrashWhenInvokedSlotConnectsNewSlot)
    {
        auto slotInvoked = false;
        auto connection = signal.connect([this, &slotInvoked]
        {
            signal.connect([&slotInvoked]
            {
                slotInvoked = true;
            });
        });
        signal.connect([]{});

        // Having two slots connected before connecting a third one
        // should cause the slots vector to reallocate invalidating
        // iterators causing a crash if a copy is not taken first.
        signal();

        // While handling a signal the slots should be immutable,
        // i.e. the newly added slot should not be invoked.
        EXPECT_FALSE(slotInvoked);

        signal();
        EXPECT_TRUE(slotInvoked);
    }

    TEST_F(SignalTest, RemoveDisconnectedSlotsBeforeConnectingNew)
    {
        const auto callable = []{}; // LCOV_EXCL_LINE
        const auto sizeofSlot = measureSizeofSlot(callable);
        ASSERT_NE(0, sizeofSlot);

        auto connection = signal.connect(callable);
        signal.connect(callable);
        connection.disconnect();

        // If the disconnected slot is not removed first,
        // the new connection will cause the slots vector to reallocate
        const auto bytesBefore = *bytesAllocated;
        signal.connect(callable);
        EXPECT_EQ(bytesBefore + sizeofSlot, *bytesAllocated);
    }
}

// Overridden operator new to spy on how many bytes are allocated
void* operator new(std::size_t count)
{
    if (auto bytes = bytesAllocated.lock(); bytes)
        *bytes += count;

    return std::malloc(count);
}
