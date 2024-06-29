// Copyright (c) 2024 Antero Nousiainen

#ifndef SIGNALS_COMBINER_HPP_
#define SIGNALS_COMBINER_HPP_

#include <functional>

namespace signals
{

template<typename R>
struct DefaultCombiner
{
    template<typename Slots, typename... Args>
    R operator()(Slots slots, Args&&... args) const
    {
        auto r = R{};

        for (auto& slot : slots)
            if (slot->connected())
                r = std::invoke(*slot, args...);

        return r;
    }
};

template<>
struct DefaultCombiner<void>
{
    template<typename Slots, typename... Args>
    void operator()(Slots slots, Args&&... args) const
    {
        for (auto& slot : slots)
            if (slot->connected())
                std::invoke(*slot, args...);
    }
};

} // namespace signals

#endif
