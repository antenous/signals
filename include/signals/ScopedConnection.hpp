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

        ScopedConnection(const Connection & connection);

        ScopedConnection(Connection && connection);

        ~ScopedConnection() override;

        ScopedConnection& operator=(const ScopedConnection&) = delete;

        ScopedConnection& operator=(ScopedConnection&&) = default;

        ScopedConnection& operator=(const Connection & connection);

        ScopedConnection& operator=(Connection && connection);

        Connection release();
    };

}

#endif
