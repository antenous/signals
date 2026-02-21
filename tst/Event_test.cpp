// Copyright (c) 2020 Antero Nousiainen

#include <signals/Event.hpp>
#include <signals/ScopedConnection.hpp>
#include <gtest/gtest.h>

namespace
{
using namespace testing;

struct TestEvent : signals::Event<TestEvent, bool(int)>
{
};

struct ReferenceEvent : signals::Event<ReferenceEvent, int&()>
{
};

template<typename R>
struct Sum
{
    template<typename Slots, typename... Args>
    auto operator()(Slots slots, Args&&... args) const
    {
        auto result = R{};

        for (auto& slot : slots)
            result += std::invoke(*slot, args...);

        return result;
    }
};

using SumSignal = signals::Signal<int(int), Sum<int>>;

struct SumEvent : signals::Event<SumEvent, int(int), SumSignal>
{
};

TEST(EventTest, InvokeSubscriberOnEvent)
{
    const signals::ScopedConnection scopedSubscription = TestEvent::subscribe([](int answer) {
        return (answer == 42);
    });
    auto event = TestEvent{};

    EXPECT_TRUE(event(42));
    EXPECT_FALSE(event(13));
}

TEST(EventTest, AllowMutatingUnderlyingValueThroughReturnedReference)
{
    auto value = 1;
    const signals::ScopedConnection scopedSubscription =
        ReferenceEvent::subscribe([&value]() -> int& {
            return value;
        });
    auto event = ReferenceEvent{};

    decltype(auto) result = event();
    result = 7;

    EXPECT_EQ(7, result);
    EXPECT_EQ(7, value);
}

TEST(EventTest, SupportCustomResultCombiner)
{
    const signals::ScopedConnection first = SumEvent::subscribe([](int n) {
        return n + 1;
    });
    const signals::ScopedConnection second = SumEvent::subscribe([](int n) {
        return n + 2;
    });
    auto event = SumEvent{};

    EXPECT_EQ(23, event(10));
}
} // namespace
