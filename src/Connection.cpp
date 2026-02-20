// Copyright (c) 2020 Antero Nousiainen

#include "signals/Connection.hpp"

namespace signals
{

Connection::Connection(const Disconnectable& slot) noexcept :
    slot(slot)
{
}

auto Connection::operator=(Connection&& other) noexcept -> Connection&
{
    if (this == &other)
        return *this;

    slot = std::move(other.slot);
    return *this;
}

auto Connection::connected() const -> bool
{
    const auto s = slot.lock();
    return s && s->connected();
}

void Connection::disconnect()
{
    if (auto s = slot.lock(); s)
        s->disconnect();
}

auto Connection::aliases(const Connection& other) const -> bool
{
    return !slot.owner_before(other.slot) && !other.slot.owner_before(slot);
}

} // namespace signals
