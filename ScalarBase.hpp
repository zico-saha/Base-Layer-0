/**
 * @file ScalarBase.hpp
 * @brief CRTP base class providing common scalar interface and algebraic queries.
 *
 * This header defines @ref Scalar::ScalarBase, a CRTP base that every concrete
 * scalar type (@ref Scalar::Real, @ref Scalar::Complex) inherits from. It
 * provides a uniform interface for value inspection, string conversion, and
 * conditionally available operations (modulus, argument, conjugate) that are
 * gated at compile time via @ref Scalar::ScalarTraits.
 *
 * @note This file should not be included directly; include @c ScalarTypes.hpp instead.
 */

#pragma once

#include "ScalarTraits.hpp"

#include <string>

namespace Scalar
{
    /**
     * @brief CRTP base class for all scalar types.
     *
     * Provides a uniform interface for common scalar operations. Derived types
     * must specialise @ref Scalar::ScalarTraits to enable or disable the
     * conditionally available operations.
     *
     * @tparam Derived The concrete scalar type that inherits from this base
     *                 (e.g. @ref Real<B> or @ref Complex<B>).
     */
    template <typename Derived>
    class ScalarBase
    {
    private:

        /**
         * @brief Returns a mutable reference to the derived type.
         * @return Reference to @p Derived.
         */
        [[nodiscard]] Derived& self() noexcept;

        /**
         * @brief Returns a const reference to the derived type.
         * @return Const reference to @p Derived.
         */
        [[nodiscard]] const Derived& self() const noexcept;

    public:

        // =====================================================================
        // Common interface
        // =====================================================================

        /**
         * @brief Returns the real part of the scalar.
         *
         * For @ref Real types this is the value itself; for @ref Complex types
         * it is the real component.
         *
         * @return The real part, with the underlying base type.
         */
        [[nodiscard]] auto real_part() const;

        /**
         * @brief Tests whether the scalar is exactly zero.
         * @return @c true if the value equals the additive identity.
         */
        [[nodiscard]] bool is_zero() const;

        /**
         * @brief Tests whether the scalar is NaN.
         *
         * For @ref Complex types, returns @c true if either component is NaN.
         *
         * @return @c true if any component is NaN.
         */
        [[nodiscard]] bool is_nan() const;

        /**
         * @brief Returns a human-readable string representation of the scalar.
         * @return A @c std::string describing the value.
         */
        [[nodiscard]] std::string to_string() const;

        // =====================================================================
        // Conditionally available operations
        // =====================================================================

        /**
         * @brief Returns the modulus (absolute value / magnitude) of the scalar.
         *
         * Only available when @c ScalarTraits<Derived>::has_modulus is @c true.
         *
         * @tparam Self Deduced as @p Derived; do not specify explicitly.
         * @return The modulus, with the underlying base type.
         */
        template <typename Self = Derived, typename = std::enable_if_t<ScalarTraits<Self>::has_modulus>>
        [[nodiscard]] auto modulus() const;

        /**
         * @brief Returns the argument (phase angle) of the scalar.
         *
         * Only available when @c ScalarTraits<Derived>::has_argument is @c true
         * (i.e. @ref Complex types only).
         *
         * @tparam Self Deduced as @p Derived; do not specify explicitly.
         * @return The argument in radians, with the underlying base type.
         */
        template <typename Self = Derived, typename = std::enable_if_t<ScalarTraits<Self>::has_argument>>
        [[nodiscard]] auto argument() const;

        /**
         * @brief Returns the complex conjugate of the scalar.
         *
         * Only available when @c ScalarTraits<Derived>::has_conjugate is @c true
         * (i.e. @ref Complex types only).
         *
         * @tparam Self Deduced as @p Derived; do not specify explicitly.
         * @return The conjugate, with the same type as @p Derived.
         */
        template <typename Self = Derived, typename = std::enable_if_t<ScalarTraits<Self>::has_conjugate>>
        [[nodiscard]] auto conjugate() const;

        // =====================================================================
        // Compile-time algebraic queries
        // =====================================================================

        /**
         * @brief Returns whether the derived type supports a total order.
         * @return @c ScalarTraits<Derived>::is_ordered.
         */
        [[nodiscard]] static constexpr bool is_ordered() noexcept;

        /**
         * @brief Returns whether the derived type has a modulus operation.
         * @return @c ScalarTraits<Derived>::has_modulus.
         */
        [[nodiscard]] static constexpr bool supports_modulus() noexcept;

        /**
         * @brief Returns whether the derived type has an argument operation.
         * @return @c ScalarTraits<Derived>::has_argument.
         */
        [[nodiscard]] static constexpr bool supports_argument() noexcept;

        /**
         * @brief Returns whether the derived type has a conjugate operation.
         * @return @c ScalarTraits<Derived>::has_conjugate.
         */
        [[nodiscard]] static constexpr bool supports_conjugate() noexcept;
    };

} // namespace Scalar

#include "ScalarBase.inl"
