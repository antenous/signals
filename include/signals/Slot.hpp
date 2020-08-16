// Copyright (c) 2020 Antero Nousiainen

#ifndef SIGNALS_SLOT_HPP_
#define SIGNALS_SLOT_HPP_

#include <functional>

namespace signals
{

    template<typename>
    class Slot;

    template<typename R, typename... Args>
    class Slot<R(Args...)>
    {
    public:
        using Callable = std::function<R(Args...)>;

        explicit Slot(Callable callable);

        Slot(const Slot&) = delete;

        Slot(Slot&&) = default;

        ~Slot() = default;

        Slot& operator=(const Slot&) = delete;

        Slot& operator=(Slot&&) = delete;

        R operator()(Args... args) const;

    private:
        Callable callable;
    };

    template<typename R, typename... Args>
    Slot<R(Args...)>::Slot(Callable callable) :
        callable(std::move(callable))
    {
    }

    template<typename R, typename... Args>
    R Slot<R(Args...)>::operator()(Args... args) const
    {
        return std::invoke(callable, args...);
    }

}

#endif
