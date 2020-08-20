// Copyright (c) 2020 Antero Nousiainen

#include "signals/Connection.hpp"

using namespace signals;

Connection::Connection(const Disconnectable & slot) :
    slot(slot)
{
}

Connection& Connection::operator=(Connection && other) noexcept
{
    std::swap(slot, other.slot);
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
