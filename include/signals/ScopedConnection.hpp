// Copyright (c) 2020 Antero Nousiainen

#ifndef SIGNALS_SCOPEDCONNECTION_HPP_
#define SIGNALS_SCOPEDCONNECTION_HPP_

#include "Connection.hpp"

namespace signals
{

class ScopedConnection : public Connection
{
public:
    ScopedConnection() = default;

    ScopedConnection(const ScopedConnection&) = delete;

    ScopedConnection(ScopedConnection&&) = default;

    ScopedConnection(const Connection& connection) noexcept;

    ScopedConnection(Connection&& connection) noexcept;

    ~ScopedConnection();

    auto operator=(const ScopedConnection&) -> ScopedConnection& = delete;

    auto operator=(ScopedConnection&& other) noexcept -> ScopedConnection&;

    auto operator=(const Connection& connection) noexcept -> ScopedConnection&;

    auto operator=(Connection&& connection) noexcept -> ScopedConnection&;

    auto release() -> Connection;

private:
    void assignAfterDisconnect(Connection connection);

    void retainAndResetAliasingSource(Connection& source);
};

} // namespace signals

#endif
