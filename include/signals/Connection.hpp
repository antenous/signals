// Copyright (c) 2020 Antero Nousiainen

#ifndef SIGNALS_CONNECTION_HPP_
#define SIGNALS_CONNECTION_HPP_

#include "Disconnectable.hpp"
#include <memory>

namespace signals
{

class Connection
{
public:
    using Disconnectable = std::shared_ptr<signals::Disconnectable>;

    Connection() = default;

    explicit Connection(const Disconnectable& slot) noexcept;

    Connection(const Connection&) = default;

    Connection(Connection&&) = default;

    ~Connection() = default;

    auto operator=(const Connection&) -> Connection& = default;

    auto operator=(Connection&& other) noexcept -> Connection&;

    [[nodiscard]] auto connected() const -> bool;

    void disconnect();

protected:
    [[nodiscard]] auto aliases(const Connection& other) const -> bool;

private:
    Disconnectable::weak_type slot;
};

} // namespace signals

#endif
