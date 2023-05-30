// Copyright (c) 2020 Antero Nousiainen

#include <signals/Disconnectable.hpp>
#include <gtest/gtest.h>

namespace signals
{
namespace
{
TEST(DisconnectableTest, HasVirtualDestructor)
{
    EXPECT_TRUE(std::has_virtual_destructor_v<Disconnectable>);
}

TEST(DisconnectableTest, IsAbstract)
{
    EXPECT_TRUE(std::is_abstract_v<Disconnectable>);
}
} // namespace
} // namespace signals
