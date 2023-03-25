// Copyright (c) 2020 Antero Nousiainen

#include <signals/Signal.hpp>
#include <gtest/gtest.h>

namespace
{
using namespace testing;

std::weak_ptr<std::size_t> bytesAllocated;

class SignalTest : public Test
{
protected:
    using Signal = signals::Signal<void()>;
    using SignalWithParams = signals::Signal<void(int&)>;

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
    SignalWithParams signalWithParams;
};

// LCOV_EXCL_START
void noop()
{
}
// LCOV_EXCL_STOP

auto multiply(int& n, int i)
{
    return [&n, i]
    {
        n *= i;
    };
}

auto multiply(int i)
{
    return [i](int& n)
    {
        n *= i;
    };
}

auto add(int& n, int i)
{
    return [&n, i]
    {
        n += i;
    };
}

auto add(int i)
{
    return [i](int& n)
    {
        n += i;
    };
}

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

TEST_F(SignalTest, IsEmptyByDefault)
{
    EXPECT_TRUE(signal.empty());
}

TEST_F(SignalTest, IsNotEmptyWhenSlotIsConnected)
{
    signal.connect(noop);
    EXPECT_FALSE(signal.empty());
}

TEST_F(SignalTest, IsEmptyWhenSlotsAreDisconnected)
{
    auto connection = signal.connect(noop);

    connection.disconnect();

    EXPECT_TRUE(signal.empty());
}

TEST_F(SignalTest, IsEmptyWhenSlotsAreCleared)
{
    signal.connect(noop);

    signal.clear();

    EXPECT_TRUE(signal.empty());
}

TEST_F(SignalTest, DoNothingOnSignalWhenNoSlotsAreConnected)
{
    signal();
}

TEST_F(SignalTest, InvokeConnectedSlotsOnSignal)
{
    auto result = 1;
    signal.connect(multiply(result, 2));
    signal.connect(add(result, 3));

    signal();

    EXPECT_EQ(5, result);
}

TEST_F(SignalTest, InvokeConnectedSlotsWithArgumentsOnSignal)
{
    auto result = 1;
    signalWithParams.connect(multiply(2));
    signalWithParams.connect(add(3));

    signalWithParams(result);

    EXPECT_EQ(5, result);
}

TEST_F(SignalTest, DoNotInvokeDisconnectedSlotOnSignal)
{
    auto result = 1;
    auto connection = signal.connect(multiply(result, 2));
    signal.connect(add(result, 3));

    connection.disconnect();

    signal();
    EXPECT_EQ(4, result);
}

TEST_F(SignalTest, DoNotCrashWhenInvokedSlotConnectsNewSlot)
{
    // clang-format off
    auto slotInvoked = false;
    signal.connect([this, &slotInvoked]
    {
        signal.connect([&slotInvoked]
        {
            slotInvoked = true;
        });
    });
    signal.connect(noop);
    // clang-format on

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
    const auto sizeofSlot = measureSizeofSlot(noop);
    ASSERT_NE(0, sizeofSlot);

    auto connection = signal.connect(noop);
    signal.connect(noop);
    connection.disconnect();

    // If the disconnected slot is not removed first,
    // the new connection will cause the slots vector to reallocate
    const auto bytesBefore = *bytesAllocated;
    signal.connect(noop);
    EXPECT_EQ(bytesBefore + sizeofSlot, *bytesAllocated);
}

TEST_F(SignalTest, ClearSourceWhenMoved)
{
    auto source = Signal{};
    const auto connection = source.connect(noop);

    const auto target = std::move(source);

    EXPECT_TRUE(source.empty());
    EXPECT_TRUE(connection.connected());
    EXPECT_FALSE(target.empty());
}

TEST_F(SignalTest, ClearSourceWhenMoveAssigned)
{
    auto source = Signal{};
    const auto sourceConnection = source.connect(noop);

    auto target = Signal{};
    const auto targetConnection = target.connect(noop);

    target = std::move(source);

    EXPECT_TRUE(source.empty());
    EXPECT_TRUE(sourceConnection.connected());
    EXPECT_FALSE(target.empty());
    EXPECT_FALSE(targetConnection.connected());
}

TEST_F(SignalTest, IsSelfMoveSafe)
{
    auto result = 1;
    signal.connect(multiply(result, 2));
    signal.connect(add(result, 3));

    const auto self = &signal;
    signal = std::move(*self);

    signal();
    EXPECT_FALSE(signal.empty());
    EXPECT_EQ(5, result);
}
} // namespace

// Overridden operator new to spy on how many bytes are allocated
void* operator new(std::size_t count)
{
    if (auto bytes = bytesAllocated.lock(); bytes)
        *bytes += count;

    return std::malloc(count);
}
