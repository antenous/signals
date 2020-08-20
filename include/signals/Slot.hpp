// Copyright (c) 2020 Antero Nousiainen

#ifndef SIGNALS_SLOT_HPP_
#define SIGNALS_SLOT_HPP_

#include <functional>
#include "Disconnectable.hpp"

namespace signals
{

    template<typename>
    class Slot;

    template<typename R, typename... Args>
    class Slot<R(Args...)> : public Disconnectable
    {
    public:
        using Callable = std::function<R(Args...)>;

        explicit Slot(Callable callable);

        Slot(const Slot&) = delete;

        Slot(Slot&&) = delete;

        ~Slot() override = default;

        Slot& operator=(const Slot&) = delete;

        Slot& operator=(Slot&&) = delete;

        R operator()(Args... args) const;

        bool connected() const override;

    private:
        void disconnect() override;

        Callable callable;
    };

    template<typename R, typename... Args>
    Slot<R(Args...)>::Slot(Callable callable) :
        callable(std::move(callable))
    {
    }

    template<typename R, typename... Args>
    bool Slot<R(Args...)>::connected() const
    {
        return callable != nullptr;
    }

    template<typename R, typename... Args>
    void Slot<R(Args...)>::disconnect()
    {
        callable = nullptr;
    }

    template<typename R, typename... Args>
    R Slot<R(Args...)>::operator()(Args... args) const
    {
        return std::invoke(callable, args...);
    }

}

#endif
