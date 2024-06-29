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

class EventTest : public Test
{
protected:
    TestEvent event;
};

TEST_F(EventTest, InvokeSubscriberOnEvent)
{
    signals::ScopedConnection scopedSubscription = TestEvent::subscribe([](int answer) {
        return (answer == 42);
    });

    EXPECT_TRUE(event(42));
    EXPECT_FALSE(event(13));
}
} // namespace
