// Copyright (c) 2024 Antero Nousiainen

#ifndef SIGNALS_COMBINER_HPP_
#define SIGNALS_COMBINER_HPP_

#include <functional>
#include <optional>
#include <type_traits>
#include <utility>

namespace signals
{

/**
 * @brief Default slot-result aggregation policy for Signal.
 *
 * For non-void signals, each connected slot is invoked in order and the
 * result from the last connected slot is returned.
 *
 * If no slots are connected:
 * - returns `R{}` when `R` is default-constructible
 * - throws `std::bad_function_call` otherwise
 *
 * Reference return types are supported; `Signal<T&(...)>` returns the last
 * connected slot's reference.
 */
template<typename R>
struct DefaultCombiner
{
private:
    using Result = std::remove_reference_t<R>;
    using Storage =
        std::conditional_t<std::is_reference_v<R>, std::reference_wrapper<Result>, Result>;

public:
    template<typename Slots, typename... Args>
    R operator()(Slots slots, Args&&... args) const
    {
        auto result = std::optional<Storage>{};

        for (auto& slot : slots)
            result = std::invoke(*slot, args...);

        return result ? unwrap(std::move(*result)) : fallback();
    }

private:
    static R unwrap(Storage&& value)
    {
        if constexpr (std::is_reference_v<R>)
            return value.get();
        else
            return std::move(value);
    }

    static R fallback()
    {
        if constexpr (std::is_default_constructible_v<R>)
            return R{};
        else
            throw std::bad_function_call{};
    }
};

template<>
struct DefaultCombiner<void>
{
    template<typename Slots, typename... Args>
    void operator()(Slots slots, Args&&... args) const
    {
        for (auto& slot : slots)
            std::invoke(*slot, args...);
    }
};

} // namespace signals

#endif
