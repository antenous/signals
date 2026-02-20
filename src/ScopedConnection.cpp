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
    if (this != &other)
        *this = other.release();

    return *this;
}

ScopedConnection& ScopedConnection::operator=(const Connection& connection) noexcept
{
    if (aliases(connection))
        return *this;

    return assignAfterDisconnect(connection);
}

ScopedConnection& ScopedConnection::operator=(Connection&& connection) noexcept
{
    if (aliases(connection))
        return retainAndResetAliasingSource(connection);

    return assignAfterDisconnect(std::move(connection));
}

ScopedConnection& ScopedConnection::assignAfterDisconnect(Connection connection)
{
    disconnect();
    Connection::operator=(std::move(connection));
    return *this;
}

ScopedConnection& ScopedConnection::retainAndResetAliasingSource(Connection& source)
{
    if (static_cast<Connection*>(this) != &source)
        source = Connection{};

    return *this;
}

Connection ScopedConnection::release()
{
    return std::move(*this);
}

} // namespace signals
