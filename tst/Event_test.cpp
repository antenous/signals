// Copyright (c) 2020 Antero Nousiainen

#include <signals/Event.hpp>
#include <signals/ScopedConnection.hpp>
#include <gtest/gtest.h>

namespace
{
using namespace testing;

struct TestEvent : signals::Event<TestEvent, void(int)>
{
};

class EventTest : public Test
{
protected:
    TestEvent event;
};

TEST_F(EventTest, InvokeSubscriberOnEvent)
{
    auto subscriberInvoked = false;

    signals::ScopedConnection scopedSubscription =
        TestEvent::subscribe([&subscriberInvoked](int answer) {
            subscriberInvoked = (answer == 42);
        });

    event(42);
    EXPECT_TRUE(subscriberInvoked);
}
} // namespace
