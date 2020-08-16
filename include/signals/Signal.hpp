// Copyright (c) 2020 Antero Nousiainen

#ifndef SIGNALS_SIGNAL_HPP_
#define SIGNALS_SIGNAL_HPP_

#include <vector>
#include "Slot.hpp"

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

        Signal& operator=(Signal && other) noexcept;

        void clear();

        bool empty() const;

        void connect(typename Slot::Callable callable);

        void operator()(Args... args) const;

    private:
        using Slots = std::vector<Slot>;

        Slots slots;
    };

    template<typename R, typename... Args>
    Signal<R(Args...)>& Signal<R(Args...)>::operator=(Signal && other) noexcept
    {
        std::swap(slots, other.slots);
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
        return slots.empty();
    }

    template<typename R, typename... Args>
    void Signal<R(Args...)>::connect(typename Slot::Callable callable)
    {
        slots.emplace_back(std::move(callable));
    }

    template<typename R, typename... Args>
    void Signal<R(Args...)>::operator()(Args... args) const
    {
        for (auto & slot : slots)
            std::invoke(slot, args...);
    }

}

#endif
