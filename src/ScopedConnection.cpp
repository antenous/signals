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

ScopedConnection& ScopedConnection::operator=(ScopedConnection&& other) noexcept
{
    if (this == &other)
        return *this;

    disconnect();
    Connection::operator=(std::move(other));
    return *this;
}

ScopedConnection& ScopedConnection::operator=(const Connection& connection) noexcept
{
    disconnect();
    Connection::operator=(connection);
    return *this;
}

ScopedConnection& ScopedConnection::operator=(Connection&& connection) noexcept
{
    disconnect();
    Connection::operator=(std::move(connection));
    return *this;
}

Connection ScopedConnection::release()
{
    return std::move(*this);
}

} // namespace signals
