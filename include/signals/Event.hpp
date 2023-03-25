// Copyright (c) 2020 Antero Nousiainen

#ifndef SIGNALS_EVENT_HPP_
#define SIGNALS_EVENT_HPP_

#include "Signal.hpp"

namespace signals
{

template<typename, typename>
class Event;

template<typename T, typename R, typename... Args>
class Event<T, R(Args...)>
{
public:
    using Signal = signals::Signal<R(Args...)>;

    static auto subscribe(typename Signal::Slot::Callable callable);

    void operator()(Args... args) const;

private:
    inline static Signal signal;
};

template<typename T, typename R, typename... Args>
auto Event<T, R(Args...)>::subscribe(typename Signal::Slot::Callable callable)
{
    return signal.connect(std::move(callable));
}

template<typename T, typename R, typename... Args>
void Event<T, R(Args...)>::operator()(Args... args) const
{
    std::invoke(signal, args...);
}

} // namespace signals

#endif
