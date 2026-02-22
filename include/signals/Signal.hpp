// Copyright (c) 2020 Antero Nousiainen

#ifndef SIGNALS_SIGNAL_HPP_
#define SIGNALS_SIGNAL_HPP_

#include "Combiner.hpp"
#include "Connection.hpp"
#include "Slot.hpp"
#include <algorithm>
#include <concepts>
#include <ranges>
#include <vector>

namespace signals
{

template<
    typename Signature, typename Combiner = DefaultCombiner<typename Slot<Signature>::Result>>
class Signal
{
public:
    using Slot = signals::Slot<Signature>;

    Signal() = default;

    Signal(const Signal&) = delete;

    Signal(Signal&&) = default;

    ~Signal() = default;

    auto operator=(const Signal&) -> Signal& = delete;

    auto operator=(Signal&& other) noexcept -> Signal&
    {
        if (this == &other)
            return *this;

        slots = std::move(other.slots);
        return *this;
    }

    void clear()
    {
        slots.clear();
    }

    [[nodiscard]] auto empty() const -> bool
    {
        return std::ranges::none_of(slots, std::mem_fn(&Slot::connected));
    }

    [[nodiscard]] auto num_slots() const
    {
        return std::ranges::count_if(slots, std::mem_fn(&Slot::connected));
    }

    auto connect(typename Slot::Callable callable)
    {
        removeDisconnectedSlots();
        return Connection{slots.emplace_back(std::make_shared<Slot>(std::move(callable)))};
    }

    template<typename... Args>
    requires std::invocable<signals::Slot<Signature>&, Args...>
    auto operator()(Args&&... args) const -> decltype(auto)
    {
        const auto immutable = slots;
        return std::invoke(
            Combiner{}, immutable | std::views::filter(std::mem_fn(&Slot::connected)),
            std::forward<Args>(args)...);
    }

private:
    using Slots = std::vector<std::shared_ptr<Slot>>;

    void removeDisconnectedSlots()
    {
        std::erase_if(slots, std::not_fn(std::mem_fn(&Slot::connected)));
    }

    Slots slots;
};

} // namespace signals

#endif
