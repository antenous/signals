// Copyright (c) 2020 Antero Nousiainen

#ifndef SIGNALS_SIGNAL_HPP_
#define SIGNALS_SIGNAL_HPP_

#include <algorithm>
#include <vector>
#include "Slot.hpp"
#include "Connection.hpp"

namespace signals
{

template<typename>
class Signal;

template<typename R, typename... Args>
class Signal<R(Args...)>
{
public:
    using Slot = signals::Slot<R(Args...)>;

    Signal() = default;

    Signal(const Signal&) = delete;

    Signal(Signal&&) = default;

    ~Signal() = default;

    Signal& operator=(const Signal&) = delete;

    Signal& operator=(Signal&& other) noexcept;

    void clear();

    bool empty() const;

    auto num_slots() const;

    auto connect(typename Slot::Callable callable);

    void operator()(Args... args) const;

private:
    using Slots = std::vector<std::shared_ptr<Slot>>;

    void removeDisconnectedSlots();

    auto immutableSlots() const;

    Slots slots;
};

template<typename R, typename... Args>
Signal<R(Args...)>& Signal<R(Args...)>::operator=(Signal&& other) noexcept
{
    if (this == &other)
        return *this;

    slots = std::move(other.slots);
    return *this;
}

template<typename R, typename... Args>
void Signal<R(Args...)>::clear()
{
    slots.clear();
}

template<typename R, typename... Args>
bool Signal<R(Args...)>::empty() const
{
    return std::none_of(std::cbegin(slots), std::cend(slots), std::mem_fn(&Slot::connected));
}

template<typename R, typename... Args>
auto Signal<R(Args...)>::num_slots() const
{
    return std::count_if(std::cbegin(slots), std::cend(slots), std::mem_fn(&Slot::connected));
}

template<typename R, typename... Args>
auto Signal<R(Args...)>::connect(typename Slot::Callable callable)
{
    removeDisconnectedSlots();
    return Connection{slots.emplace_back(std::make_shared<Slot>(std::move(callable)))};
}

template<typename R, typename... Args>
void Signal<R(Args...)>::removeDisconnectedSlots()
{
    slots.erase(
        std::remove_if(
            std::begin(slots), std::end(slots), std::not_fn(std::mem_fn(&Slot::connected))),
        slots.end());
}

template<typename R, typename... Args>
void Signal<R(Args...)>::operator()(Args... args) const
{
    for (auto& slot : immutableSlots())
        if (slot->connected())
            std::invoke(*slot, args...);
}

template<typename R, typename... Args>
auto Signal<R(Args...)>::immutableSlots() const
{
    return slots;
}

} // namespace signals

#endif
