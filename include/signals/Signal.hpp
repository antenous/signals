// Copyright (c) 2020 Antero Nousiainen

#ifndef SIGNALS_SIGNAL_HPP_
#define SIGNALS_SIGNAL_HPP_

#include "Combiner.hpp"
#include "Connection.hpp"
#include "Slot.hpp"
#include <algorithm>
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

    Signal& operator=(const Signal&) = delete;

    Signal& operator=(Signal&& other) noexcept;

    void clear();

    [[nodiscard]] bool empty() const;

    [[nodiscard]] auto num_slots() const;

    auto connect(typename Slot::Callable callable);

    template<typename... Args>
    auto operator()(Args&&... args) const;

private:
    using Slots = std::vector<std::shared_ptr<Slot>>;

    void removeDisconnectedSlots();

    Slots slots;
};

template<typename Signature, typename Combiner>
auto Signal<Signature, Combiner>::operator=(Signal&& other) noexcept -> Signal&
{
    if (this == &other)
        return *this;

    slots = std::move(other.slots);
    return *this;
}

template<typename Signature, typename Combiner>
void Signal<Signature, Combiner>::clear()
{
    slots.clear();
}

template<typename Signature, typename Combiner>
bool Signal<Signature, Combiner>::empty() const
{
    return std::ranges::none_of(slots, std::mem_fn(&Slot::connected));
}

template<typename Signature, typename Combiner>
auto Signal<Signature, Combiner>::num_slots() const
{
    return std::ranges::count_if(slots, std::mem_fn(&Slot::connected));
}

template<typename Signature, typename Combiner>
auto Signal<Signature, Combiner>::connect(typename Slot::Callable callable)
{
    removeDisconnectedSlots();
    return Connection{slots.emplace_back(std::make_shared<Slot>(std::move(callable)))};
}

template<typename Signature, typename Combiner>
void Signal<Signature, Combiner>::removeDisconnectedSlots()
{
    std::erase_if(slots, std::not_fn(std::mem_fn(&Slot::connected)));
}

template<typename Signature, typename Combiner>
template<typename... Args>
inline auto Signal<Signature, Combiner>::operator()(Args&&... args) const
{
    const auto immutable = slots;
    return std::invoke(
        Combiner{}, immutable | std::views::filter(std::mem_fn(&Slot::connected)),
        std::forward<Args>(args)...);
}

} // namespace signals

#endif
