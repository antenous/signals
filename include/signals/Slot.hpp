// Copyright (c) 2020 Antero Nousiainen

#ifndef SIGNALS_SLOT_HPP_
#define SIGNALS_SLOT_HPP_

#include "Disconnectable.hpp"
#include <concepts>
#include <functional>
#include <type_traits>

namespace signals
{

namespace detail
{
template<typename T>
concept is_complete_type = requires { sizeof(T); };

template<typename T>
concept is_copy_constructible_if_complete =
    (!is_complete_type<T>) || std::is_copy_constructible_v<T>;
} // namespace detail

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
 * By-value parameters must be copy-constructible so equivalent argument values
 * can be delivered to all connected slots.
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
    static_assert(
        ((std::is_reference_v<Args> || detail::is_copy_constructible_if_complete<Args>)&&...),
        "signals::Slot requires by-value parameters to be copy constructible");

    using Callable = std::function<R(Args...)>;

    using Result = R;

    explicit Slot(Callable callable);

    Slot(const Slot&) = delete;

    Slot(Slot&&) = delete;

    ~Slot() override = default;

    auto operator=(const Slot&) -> Slot& = delete;

    auto operator=(Slot&&) -> Slot& = delete;

    auto operator()(Args... args) const -> R
    requires((std::is_reference_v<Args> || std::is_copy_constructible_v<Args>) && ...);

    [[nodiscard]] auto connected() const -> bool override;

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
auto Slot<R(Args...)>::connected() const -> bool
{
    return callable != nullptr;
}

template<typename R, typename... Args>
void Slot<R(Args...)>::disconnect()
{
    callable = nullptr;
}

template<typename R, typename... Args>
auto Slot<R(Args...)>::operator()(Args... args) const -> R
requires((std::is_reference_v<Args> || std::is_copy_constructible_v<Args>) && ...)
{
    return std::invoke(callable, args...);
}

} // namespace signals

#endif
