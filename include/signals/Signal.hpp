// Copyright (c) 2020 Antero Nousiainen

#ifndef SIGNALS_SIGNAL_HPP_
#define SIGNALS_SIGNAL_HPP_

#include "Slot.hpp"
#include "Connection.hpp"
#include <algorithm>
#include <vector>

namespace signals
{

template<typename Signature>
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

    bool empty() const;

    auto num_slots() const;

    auto connect(typename Slot::Callable callable);

    template<typename... Args>
    void operator()(Args&&... args) const;

private:
    using Slots = std::vector<std::shared_ptr<Slot>>;

    void removeDisconnectedSlots();

    auto immutableSlots() const;

    Slots slots;
};

template<typename Signature>
auto Signal<Signature>::operator=(Signal&& other) noexcept -> Signal&
{
    if (this == &other)
        return *this;

    slots = std::move(other.slots);
    return *this;
}

template<typename Signature>
void Signal<Signature>::clear()
{
    slots.clear();
}

template<typename Signature>
bool Signal<Signature>::empty() const
{
    return std::none_of(std::cbegin(slots), std::cend(slots), std::mem_fn(&Slot::connected));
}

template<typename Signature>
auto Signal<Signature>::num_slots() const
{
    return std::count_if(std::cbegin(slots), std::cend(slots), std::mem_fn(&Slot::connected));
}

template<typename Signature>
auto Signal<Signature>::connect(typename Slot::Callable callable)
{
    removeDisconnectedSlots();
    return Connection{slots.emplace_back(std::make_shared<Slot>(std::move(callable)))};
}

template<typename Signature>
void Signal<Signature>::removeDisconnectedSlots()
{
    slots.erase(
        std::remove_if(
            std::begin(slots), std::end(slots), std::not_fn(std::mem_fn(&Slot::connected))),
        slots.end());
}

template<typename Signature>
template<typename... Args>
inline void Signal<Signature>::operator()(Args&&... args) const
{
    for (auto& slot : immutableSlots())
        if (slot->connected())
            std::invoke(*slot, args...);
}

template<typename Signature>
auto Signal<Signature>::immutableSlots() const
{
    return slots;
}

} // namespace signals

#endif
