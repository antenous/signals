// Copyright (c) 2020 Antero Nousiainen

#include <signals/Signal.hpp>
#include <gmock/gmock.h>
#include <cstdlib>
#include <vector>

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

    [[nodiscard]] auto measureSizeofSlot(typename Signal::Slot::Callable callable) const
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
    return [&n, i] {
        n *= i;
    };
}

auto multiply(int i)
{
    return [i](int& n) {
        n *= i;
    };
}

auto add(int& n, int i)
{
    return [&n, i] {
        n += i;
    };
}

auto add(int i)
{
    return [i](int& n) {
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
    EXPECT_EQ(0, signal.num_slots());
}

TEST_F(SignalTest, IsNotEmptyWhenSlotIsConnected)
{
    signal.connect(noop);
    EXPECT_FALSE(signal.empty());
    EXPECT_EQ(1, signal.num_slots());
}

TEST_F(SignalTest, IsEmptyWhenSlotsAreDisconnected)
{
    auto connection = signal.connect(noop);

    connection.disconnect();

    EXPECT_TRUE(signal.empty());
    EXPECT_EQ(0, signal.num_slots());
}

TEST_F(SignalTest, IsEmptyWhenSlotsAreCleared)
{
    signal.connect(noop);

    signal.clear();

    EXPECT_TRUE(signal.empty());
    EXPECT_EQ(0, signal.num_slots());
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

TEST_F(SignalTest, RejectRvalueArgumentForLvalueReferenceParameter)
{
    EXPECT_FALSE((std::is_invocable_v<SignalWithParams&, int>));
}

TEST_F(SignalTest, InvokeConnectedSlotsWithMultipleArgumentsOnSignal)
{
    auto result = 0;
    auto multi = signals::Signal<void(int&, int)>{};

    multi.connect([](int& i, int j) {
        i += j;
    });

    multi(result, 42);

    EXPECT_EQ(42, result);
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

TEST_F(SignalTest, DoNotInvokeDisconnectedSlotOnSignalWhenDisconnectedDuringSignal)
{
    auto result = 1;
    auto connection = signals::Connection{};

    signal.connect([&connection, &result] {
        connection.disconnect();
        result *= 2;
    });
    connection = signal.connect(add(result, 3));

    signal();
    EXPECT_EQ(2, result);
}

TEST_F(SignalTest, DoNotCrashWhenInvokedSlotConnectsNewSlot)
{
    auto slotInvoked = false;
    signal.connect([this, &slotInvoked] {
        signal.connect([&slotInvoked] {
            slotInvoked = true;
        });
    });
    signal.connect(noop);

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

struct MoveSentinel
{
    explicit MoveSentinel(int value) :
        value(value)
    {
    }

    MoveSentinel(const MoveSentinel&) = default;
    auto operator=(const MoveSentinel&) -> MoveSentinel& = default;

    MoveSentinel(MoveSentinel&& other) noexcept :
        value(other.value)
    {
        other.value = -1;
    }

    auto operator=(MoveSentinel&& other) noexcept -> MoveSentinel&
    {
        if (this == &other)
            return *this;

        value = other.value;
        other.value = -1;
        return *this;
    }

    ~MoveSentinel() = default;

    int value = 0;
};

TEST_F(SignalTest, DeliverEquivalentValueToMultipleSlotsWhenSignalingRvalue)
{
    // This test verifies that when a signal is emitted with an rvalue argument, the argument
    // is copied for each slot, and each slot receives an equivalent value. If the argument
    // were moved instead of copied, only the first slot would receive the correct value, and
    // subsequent slots would receive a moved-from object.

    auto fanoutSignal = signals::Signal<void(MoveSentinel)>{};
    auto seen = std::vector<int>{};

    fanoutSignal.connect([&seen](MoveSentinel sentinel) {
        seen.push_back(sentinel.value);
    });
    fanoutSignal.connect([&seen](MoveSentinel sentinel) {
        seen.push_back(sentinel.value);
    });

    fanoutSignal(MoveSentinel{7});

    EXPECT_THAT(seen, ElementsAre(7, 7));
}

TEST_F(SignalTest, ReturnLastValueWhenDefaultCombinerIsUsed)
{
    auto last = signals::Signal<int()>{};

    // clang-format off
    last.connect([]{ return 1; });
    last.connect([]{ return 2; });
    last.connect([]{ return 3; });
    // clang-format on

    EXPECT_EQ(3, last());
}

TEST_F(SignalTest, ReturnDefaultConstructedValueWhenNoConnectedSlotsExist)
{
    auto last = signals::Signal<int()>{};
    // LCOV_EXCL_START
    auto connection = last.connect([] {
        return 42;
    });
    // LCOV_EXCL_STOP
    connection.disconnect();

    EXPECT_EQ(0, last());
}

struct NonDefaultResult
{
    explicit NonDefaultResult(int value) :
        value(value)
    {
    }

    int value = 0;
};

TEST_F(SignalTest, SupportNonDefaultConstructibleSlotResultWithDefaultCombiner)
{
    auto last = signals::Signal<NonDefaultResult()>{};
    last.connect([] {
        return NonDefaultResult{42};
    });

    EXPECT_EQ(42, last().value);
}

TEST_F(SignalTest, ThrowWhenNoConnectedSlotsAndResultIsNotDefaultConstructible)
{
    auto last = signals::Signal<NonDefaultResult()>{};

    EXPECT_THROW(last(), std::bad_function_call);
}

TEST_F(SignalTest, SupportReferenceSlotResultWithDefaultCombiner)
{
    auto first = 1;
    auto second = 2;
    auto last = signals::Signal<int&()>{};

    last.connect([&first]() -> int& {
        return first;
    });

    last.connect([&second]() -> int& {
        return second;
    });

    auto& result = last();
    result = 7;

    EXPECT_EQ(1, first);
    EXPECT_EQ(7, second);
}

template<typename R>
struct Sum
{
    template<typename Slots, typename... Args>
    auto operator()(Slots slots, Args&&... args) const
    {
        auto r = R{};

        for (auto& slot : slots)
            r += std::invoke(*slot, args...);

        return r;
    }
};

TEST_F(SignalTest, SupportCustomResultCombiner)
{
    auto sum = signals::Signal<int(), Sum<int>>{};

    // clang-format off
    sum.connect([]{ return 1; });
    sum.connect([]{ return 2; });
    sum.connect([]{ return 3; });
    // clang-format on

    EXPECT_EQ(6, sum());
}

template<typename R>
struct Collector
{
    template<typename Slots, typename... Args>
    auto operator()(Slots slots, Args&&... args) const
    {
        auto r = R{};

        for (auto& slot : slots)
            r.push_back(std::invoke(*slot, args...));

        return r;
    }
};

TEST_F(SignalTest, SupportArbitraryCombinerResultType)
{
    auto collection = signals::Signal<int(), Collector<std::vector<int>>>{};

    // clang-format off
    collection.connect([]{ return 1; });
    collection.connect([]{ return 2; });
    collection.connect([]{ return 3; });
    // clang-format on

    EXPECT_THAT(collection(), ElementsAre(1, 2, 3));
}
} // namespace

// Overridden operator new to spy on how many bytes are allocated
auto operator new(std::size_t count) -> void*
{
    if (auto bytes = bytesAllocated.lock(); bytes)
        *bytes += count;

    return std::malloc(count);
}

void operator delete(void* ptr) noexcept
{
    std::free(ptr);
}

void operator delete(void* ptr, [[maybe_unused]] std::size_t size) noexcept
{
    std::free(ptr);
}
