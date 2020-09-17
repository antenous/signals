// Copyright (c) 2020 Antero Nousiainen

#include "signals/ScopedConnection.hpp"

using namespace signals;

ScopedConnection::ScopedConnection(const Connection & connection) :
    Connection(connection)
{
}

ScopedConnection::ScopedConnection(Connection && connection) :
    Connection(std::move(connection))
{
}

ScopedConnection::~ScopedConnection()
{
    disconnect();
}

ScopedConnection& ScopedConnection::operator=(const Connection & connection)
{
    disconnect();
    Connection::operator=(connection);
    return *this;
}

ScopedConnection& ScopedConnection::operator=(Connection && connection)
{
    disconnect();
    Connection::operator=(std::move(connection));
    return *this;
}

Connection ScopedConnection::release()
{
    return std::move(*this);
}
