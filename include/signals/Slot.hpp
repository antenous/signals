// Copyright (c) 2020 Antero Nousiainen

#ifndef SIGNALS_SLOT_HPP_
#define SIGNALS_SLOT_HPP_

#include "Disconnectable.hpp"
#include <functional>
#include <type_traits>

namespace signals
{

template<typename>
class Slot;

/**
 * @brief Type-erased callable wrapper used by Signal.
 *
 * Rvalue-reference parameters in the signature are intentionally unsupported.
 * Slots are dispatched through type-erased callables, and call arguments are
 * materialized before invocation, so preserving `T&&` signature semantics is
 * not reliable across dispatch. Allowing `T&&` in the slot signature would
 * therefore suggest move-only call contracts that this API cannot guarantee.
 *
 * Use by-value, `const T&`, or `T&` parameters instead.
 */
template<typename R, typename... Args>
class Slot<R(Args...)> : public Disconnectable
{
public:
    static_assert(
        (!std::is_rvalue_reference_v<Args> && ...),
        "signals::Slot does not support rvalue-reference parameters in signatures");

    using Callable = std::function<R(Args...)>;

    using Result = R;

    explicit Slot(Callable callable);

    Slot(const Slot&) = delete;

    Slot(Slot&&) = delete;

    ~Slot() override = default;

    Slot& operator=(const Slot&) = delete;

    Slot& operator=(Slot&&) = delete;

    R operator()(Args... args) const;

    [[nodiscard]] bool connected() const override;

private:
    void disconnect() override;

    Callable callable;
};

template<typename R, typename... Args>
Slot<R(Args...)>::Slot(Callable callable) :
    callable(std::move(callable))
{
}

template<typename R, typename... Args>
bool Slot<R(Args...)>::connected() const
{
    return callable != nullptr;
}

template<typename R, typename... Args>
void Slot<R(Args...)>::disconnect()
{
    callable = nullptr;
}

template<typename R, typename... Args>
R Slot<R(Args...)>::operator()(Args... args) const
{
    return std::invoke(callable, args...);
}

} // namespace signals

#endif
