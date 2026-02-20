// Copyright (c) 2020 Antero Nousiainen

#include "signals/ScopedConnection.hpp"

namespace signals
{

ScopedConnection::ScopedConnection(const Connection& connection) noexcept :
    Connection(connection)
{
}

ScopedConnection::ScopedConnection(Connection&& connection) noexcept :
    Connection(std::move(connection))
{
}

ScopedConnection::~ScopedConnection()
{
    disconnect();
}

auto ScopedConnection::operator=(ScopedConnection&& other) noexcept -> ScopedConnection&
{
    if (this == &other)
        return *this;

    *this = other.release();
    return *this;
}

auto ScopedConnection::operator=(const Connection& connection) noexcept -> ScopedConnection&
{
    if (aliases(connection))
        return *this;

    assignAfterDisconnect(connection);
    return *this;
}

auto ScopedConnection::operator=(Connection&& connection) noexcept -> ScopedConnection&
{
    if (aliases(connection))
        retainAndResetAliasingSource(connection);
    else
        assignAfterDisconnect(std::move(connection));

    return *this;
}

void ScopedConnection::assignAfterDisconnect(Connection connection)
{
    disconnect();
    Connection::operator=(std::move(connection));
}

void ScopedConnection::retainAndResetAliasingSource(Connection& source)
{
    if (static_cast<Connection*>(this) == &source)
        return;

    source = Connection{};
}

auto ScopedConnection::release() -> Connection
{
    return std::move(*this);
}

} // namespace signals
