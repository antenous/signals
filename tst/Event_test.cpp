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

TEST(EventTest, InvokeSubscriberOnEvent)
{
    signals::ScopedConnection scopedSubscription = TestEvent::subscribe([](int answer) {
        return (answer == 42);
    });
    auto event = TestEvent{};

    EXPECT_TRUE(event(42));
    EXPECT_FALSE(event(13));
}

TEST(EventTest, AllowMutatingUnderlyingValueThroughReturnedReference)
{
    auto value = 1;
    signals::ScopedConnection scopedSubscription = ReferenceEvent::subscribe([&value]() -> int& {
        return value;
    });
    auto event = ReferenceEvent{};

    decltype(auto) result = event();
    result = 7;

    EXPECT_EQ(7, result);
    EXPECT_EQ(7, value);
}
} // namespace
