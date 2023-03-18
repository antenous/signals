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

        ScopedConnection(const Connection & connection) noexcept;

        ScopedConnection(Connection && connection) noexcept;

        ~ScopedConnection() override;

        ScopedConnection& operator=(const ScopedConnection&) = delete;

        ScopedConnection& operator=(ScopedConnection && other) noexcept;

        ScopedConnection& operator=(const Connection & connection) noexcept;

        ScopedConnection& operator=(Connection && connection) noexcept;

        Connection release();
    };

}

#endif
