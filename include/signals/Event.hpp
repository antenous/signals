// Copyright (c) 2020 Antero Nousiainen

#ifndef SIGNALS_EVENT_HPP_
#define SIGNALS_EVENT_HPP_

#include "Signal.hpp"

namespace signals
{

template<typename T, typename Signature>
class Event
{
public:
    template<typename Fn>
    static auto subscribe(Fn&& fn)
    {
        return signal.connect(std::forward<Fn>(fn));
    }

    template<typename... Args>
    auto operator()(Args&&... args) const -> decltype(auto)
    {
        return std::invoke(signal, std::forward<Args>(args)...);
    }

private:
    static inline Signal<Signature> signal;
};

} // namespace signals

#endif
