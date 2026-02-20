// Copyright (c) 2020 Antero Nousiainen

#ifndef SIGNALS_DISCONNECTABLE_HPP_
#define SIGNALS_DISCONNECTABLE_HPP_

namespace signals
{

class Disconnectable
{
public:
    virtual ~Disconnectable() = default;

    [[nodiscard]] virtual auto connected() const -> bool = 0;

    virtual void disconnect() = 0;
};

} // namespace signals

#endif
