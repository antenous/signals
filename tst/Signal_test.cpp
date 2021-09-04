// Copyright (c) 2020 Antero Nousiainen

#include <signals/Signal.hpp>
#include <gtest/gtest.h>

using namespace signals;

namespace
{
    TEST(SignalTest, IsNoncopyable)
    {
        EXPECT_FALSE(std::is_copy_constructible_v<Signal>);
        EXPECT_FALSE(std::is_copy_assignable_v<Signal>);
    }
}
