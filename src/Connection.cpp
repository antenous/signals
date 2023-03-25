// Copyright (c) 2020 Antero Nousiainen

#include "signals/Connection.hpp"

namespace signals
{

Connection::Connection(const Disconnectable& slot) noexcept :
    slot(slot)
{
}

Connection& Connection::operator=(Connection&& other) noexcept
{
    if (this == &other)
        return *this;

    slot = std::move(other.slot);
    return *this;
}

bool Connection::connected() const
{
    const auto s = slot.lock();
    return s && s->connected();
}

void Connection::disconnect()
{
    if (auto s = slot.lock(); s)
        s->disconnect();
}

} // namespace signals
