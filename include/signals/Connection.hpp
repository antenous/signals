// Copyright (c) 2020 Antero Nousiainen

#ifndef SIGNALS_CONNECTION_HPP_
#define SIGNALS_CONNECTION_HPP_

#include <memory>
#include "Disconnectable.hpp"

namespace signals
{

    class Connection
    {
    public:
        using Disconnectable = std::shared_ptr<signals::Disconnectable>;

        Connection() = default;

        explicit Connection(const Disconnectable & slot);

        Connection(const Connection&) = default;

        Connection(Connection&&) = default;

        Connection& operator=(const Connection&) = default;

        Connection& operator=(Connection && other) noexcept;

        ~Connection() = default;

        bool connected() const;

        void disconnect();

    private:
        Disconnectable::weak_type slot;
    };

}

#endif
