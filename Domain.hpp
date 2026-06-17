/**
 * @file Domain.hpp
 * @brief Domain wrapper providing named-type semantics over scalar types.
 *
 * This header defines @ref Scalar::Domain, a thin CRTP-aware wrapper that
 * associates a mathematical domain (e.g. frequency, voltage, time) with an
 * underlying scalar type (@ref Scalar::Real or @ref Scalar::Complex). It
 * forwards the full arithmetic, comparison, and analytic interface of the
 * wrapped scalar while enforcing domain-level type safety at compile time.
 *
 * Convenience aliases are provided for the two most common instantiations:
 * - @ref Scalar::RealScalar<B>    — @c Domain<Real<B>>
 * - @ref Scalar::ComplexScalar<B> — @c Domain<Complex<B>>
 *
 * @note Include this file (or the umbrella header) rather than individual
 *       component headers.
 */

#pragma once

#include "ScalarTypes.hpp"

#include <cmath>
#include <ostream>
#include <stdexcept>
#include <type_traits>

namespace Scalar
{
    /**
     * @brief Named-domain wrapper around a scalar type.
     *
     * @ref Domain<IS> wraps an inner scalar value of type @p IS and re-exposes
     * its full interface — arithmetic, comparison, CRTP-delegated analytic
     * operations, and stream output — while preserving domain identity in the
     * type system. Mixed-domain arithmetic is possible through the promotion
     * rules defined in @ref Scalar::Promote.
     *
     * @tparam IS The inner scalar type. Must derive from @ref ScalarBase<IS>
     *            (i.e. must be @ref Real<B> or @ref Complex<B>).
     *
     * @par Example
     * @code
     * using Frequency = Domain<Real<double>>;
     * using Voltage   = Domain<Complex<double>>;
     *
     * Frequency f(440.0);
     * Voltage   v(1.0, 0.5);
     * auto      result = v * f;   // Domain<Complex<double>>
     * @endcode
     */
    template <typename IS>
    class Domain
    {
        static_assert(std::is_base_of_v<ScalarBase<IS>, IS>, "Domain<IS>: IS must derive from ScalarBase<IS>.");

    private:
        IS inner_{}; ///< The wrapped scalar value.

    public:
        /// The inner scalar type (e.g. @ref Real<double>, @ref Complex<float>).
        using inner_type = IS;

        /// The underlying floating-point base type, as reported by @ref ScalarTraits.
        using value_type = typename ScalarTraits<IS>::base_type;

        // =====================================================================
        // Constructors
        // =====================================================================

        /**
         * @brief Default constructor. Value-initialises the inner scalar.
         */
        constexpr Domain() = default;

        /**
         * @brief Constructs from a const reference to an inner scalar.
         * @param _value The inner scalar value to copy.
         */
        constexpr explicit Domain(const IS &_value);

        /**
         * @brief Constructs from an rvalue reference to an inner scalar.
         * @param _value The inner scalar value to move.
         */
        constexpr explicit Domain(IS &&_value);

        /**
         * @brief Constructs from a raw numeric value.
         *
         * The numeric value is forwarded to the inner scalar's constructor.
         *
         * @tparam ES Raw numeric type; must satisfy @ref is_numeric_v.
         * @param _value The numeric value to wrap.
         */
        template <typename ES, typename = std::enable_if_t<is_numeric_v<ES>>>
        constexpr explicit Domain(const ES &_value);

        /**
         * @brief Forwarding constructor for multi-argument inner scalar construction.
         *
         * Enables constructs such as @c Domain<Complex<double>>(re, im) by
         * perfect-forwarding all arguments to @p IS.
         *
         * @tparam Args Argument types; @p IS must be constructible from @p Args.
         * @param _args Arguments forwarded to the @p IS constructor.
         */
        template <typename... Args, typename = std::enable_if_t<std::is_constructible_v<IS, Args...>>>
        constexpr explicit Domain(Args &&..._args);

        // =====================================================================
        // Accessors
        // =====================================================================

        /**
         * @brief Returns a const reference to the inner scalar.
         * @return Const reference to the wrapped @p IS value.
         */
        [[nodiscard]] const IS &inner() const noexcept;

        /**
         * @brief Returns a mutable reference to the inner scalar.
         * @return Reference to the wrapped @p IS value.
         */
        [[nodiscard]] IS &inner() noexcept;

        // =====================================================================
        // ScalarBase interface delegation
        // =====================================================================

        /**
         * @brief Returns the real part of the inner scalar.
         * @return The real component, with the underlying base type.
         */
        [[nodiscard]] auto real_part() const;

        /**
         * @brief Tests whether the inner scalar is exactly zero.
         * @return @c true if the value equals the additive identity.
         */
        [[nodiscard]] bool is_zero() const;

        /**
         * @brief Tests whether the inner scalar is NaN.
         *
         * For @ref Complex inner types, returns @c true if either component is NaN.
         *
         * @return @c true if any component is NaN.
         */
        [[nodiscard]] bool is_nan() const;

        /**
         * @brief Returns a human-readable string representation.
         * @return A @c std::string describing the inner value.
         */
        [[nodiscard]] std::string to_string() const;

        /**
         * @brief Returns the modulus of the inner scalar wrapped in a real domain.
         *
         * Only available when @c ScalarTraits<IS>::has_modulus is @c true.
         *
         * @tparam S Deduced as @p IS; do not specify explicitly.
         * @return @c Domain<Real<value_type>> holding the modulus.
         */
        template <typename S = IS, typename = std::enable_if_t<ScalarTraits<S>::has_modulus>>
        [[nodiscard]] Domain<Real<value_type>> modulus() const;

        /**
         * @brief Returns the argument (phase angle) of the inner scalar wrapped in a real domain.
         *
         * Only available when @c ScalarTraits<IS>::has_argument is @c true
         * (i.e. @ref Complex inner types only).
         *
         * @tparam S Deduced as @p IS; do not specify explicitly.
         * @return @c Domain<Real<value_type>> holding the argument in radians.
         */
        template <typename S = IS, typename = std::enable_if_t<ScalarTraits<S>::has_argument>>
        [[nodiscard]] Domain<Real<value_type>> argument() const;

        /**
         * @brief Returns the complex conjugate of the inner scalar.
         *
         * Only available when @c ScalarTraits<IS>::has_conjugate is @c true
         * (i.e. @ref Complex inner types only).
         *
         * @tparam S Deduced as @p IS; do not specify explicitly.
         * @return A @c Domain<IS> holding the conjugate value.
         */
        template <typename S = IS, typename = std::enable_if_t<ScalarTraits<S>::has_conjugate>>
        [[nodiscard]] Domain<IS> conjugate() const;

        // =====================================================================
        // Tolerance-based zero check
        // =====================================================================

        /**
         * @brief Tests whether the inner scalar is zero within a given tolerance.
         *
         * For @ref Real types, checks @c |value| <= tolerance.
         * For @ref Complex types, checks @c modulus() <= tolerance.
         *
         * @tparam EB Tolerance type; must satisfy @ref is_numeric_v.
         * @param _tolerance Maximum acceptable deviation from zero.
         * @return @c true if the value is within @p _tolerance of zero.
         */
        template <typename EB, typename = std::enable_if_t<is_numeric_v<EB>>>
        [[nodiscard]] bool is_zero(const EB &_tolerance) const;

        // =====================================================================
        // Conversion operator
        // =====================================================================

        /**
         * @brief Implicit conversion to a @ref Domain with a compatible inner type.
         *
         * The conversion is permitted only when @ref IsAllowedConversion<IS, ES>
         * is @c true and @p ES is constructible from @p IS. Notably, conversion
         * from @c Domain<Complex> to @c Domain<Real> is disallowed.
         *
         * @tparam ES Target inner scalar type.
         */
        template <typename ES, typename = std::enable_if_t<IsAllowedConversion<IS, ES>::value && std::is_constructible_v<ES, IS>>>
        operator Domain<ES>() const;

        // =====================================================================
        // Unary operators
        // =====================================================================

        /// @brief Unary plus. Returns a copy of this domain value.
        [[nodiscard]] Domain operator+() const;

        /// @brief Unary minus. Returns the negation of this domain value.
        [[nodiscard]] Domain operator-() const;

        // =====================================================================
        // Arithmetic operators — Domain<ES> operand
        // =====================================================================

        /**
         * @brief Adds two domain scalars, promoting to the common scalar type.
         */
        template <typename ES>
        [[nodiscard]] Domain<promote_t<IS, ES>> operator+(const Domain<ES> &_scalar) const;

        /**
         * @brief Subtracts two domain scalars, promoting to the common scalar type.
         */
        template <typename ES>
        [[nodiscard]] Domain<promote_t<IS, ES>> operator-(const Domain<ES> &_scalar) const;

        /**
         * @brief Multiplies two domain scalars, promoting to the common scalar type.
         */
        template <typename ES>
        [[nodiscard]] Domain<promote_t<IS, ES>> operator*(const Domain<ES> &_scalar) const;

        /**
         * @brief Divides two domain scalars, promoting to the common scalar type.
         */
        template <typename ES>
        [[nodiscard]] Domain<promote_t<IS, ES>> operator/(const Domain<ES> &_scalar) const;

        /**
         * @brief Computes the floating-point remainder of two domain scalars.
         *
         * Only available when both @p IS and @p ES support the modulo operation
         * (@c MathTraits<IS>::supports_modulo_op and @c MathTraits<ES>::supports_modulo_op).
         */
        template <typename ES, typename = std::enable_if_t<MathTraits<IS>::supports_modulo_op && MathTraits<ES>::supports_modulo_op>>
        [[nodiscard]] Domain<promote_t<IS, ES>> operator%(const Domain<ES> &_scalar) const;

        // =====================================================================
        // Arithmetic operators — raw numeric operand
        // =====================================================================

        /**
         * @brief Adds a raw numeric value to this domain scalar.
         */
        template <typename EB, typename = std::enable_if_t<is_numeric_v<EB>>>
        [[nodiscard]] Domain<promote_t<IS, Real<to_float_t<EB>>>> operator+(const EB &_value) const;

        /**
         * @brief Subtracts a raw numeric value from this domain scalar.
         */
        template <typename EB, typename = std::enable_if_t<is_numeric_v<EB>>>
        [[nodiscard]] Domain<promote_t<IS, Real<to_float_t<EB>>>> operator-(const EB &_value) const;

        /**
         * @brief Multiplies this domain scalar by a raw numeric value.
         */
        template <typename EB, typename = std::enable_if_t<is_numeric_v<EB>>>
        [[nodiscard]] Domain<promote_t<IS, Real<to_float_t<EB>>>> operator*(const EB &_value) const;

        /**
         * @brief Divides this domain scalar by a raw numeric value.
         */
        template <typename EB, typename = std::enable_if_t<is_numeric_v<EB>>>
        [[nodiscard]] Domain<promote_t<IS, Real<to_float_t<EB>>>> operator/(const EB &_value) const;

        /**
         * @brief Computes the floating-point remainder with a raw numeric value.
         *
         * Only available when @p IS supports the modulo operation
         * (@c MathTraits<IS>::supports_modulo_op).
         */
        template <typename EB, typename = std::enable_if_t<MathTraits<IS>::supports_modulo_op && is_numeric_v<EB>>>
        [[nodiscard]] Domain<promote_t<IS, Real<to_float_t<EB>>>> operator%(const EB &_value) const;

        // =====================================================================
        // Compound assignment operators — Domain<ES> operand
        // =====================================================================

        /**
         * @brief Adds @p _scalar to this value in place.
         */
        template <typename ES>
        Domain<IS> &operator+=(const Domain<ES> &_scalar);

        /**
         * @brief Subtracts @p _scalar from this value in place.
         */
        template <typename ES>
        Domain<IS> &operator-=(const Domain<ES> &_scalar);

        /**
         * @brief Multiplies this value by @p _scalar in place.
         */
        template <typename ES>
        Domain<IS> &operator*=(const Domain<ES> &_scalar);

        /**
         * @brief Divides this value by @p _scalar in place.
         */
        template <typename ES>
        Domain<IS> &operator/=(const Domain<ES> &_scalar);

        /**
         * @brief Applies the floating-point remainder with @p _scalar in place.
         *
         * Only available when both @p IS and @p ES support the modulo operation.
         */
        template <typename ES, typename = std::enable_if_t<MathTraits<IS>::supports_modulo_op && MathTraits<ES>::supports_modulo_op>>
        Domain<IS> &operator%=(const Domain<ES> &_scalar);

        // =====================================================================
        // Compound assignment operators — raw numeric operand
        // =====================================================================

        /**
         * @brief Adds a raw numeric value to this scalar in place.
         */
        template <typename EB, typename = std::enable_if_t<is_numeric_v<EB>>>
        Domain<IS> &operator+=(const EB &_value);

        /**
         * @brief Subtracts a raw numeric value from this scalar in place.
         */
        template <typename EB, typename = std::enable_if_t<is_numeric_v<EB>>>
        Domain<IS> &operator-=(const EB &_value);

        /**
         * @brief Multiplies this scalar by a raw numeric value in place.
         */
        template <typename EB, typename = std::enable_if_t<is_numeric_v<EB>>>
        Domain<IS> &operator*=(const EB &_value);

        /**
         * @brief Divides this scalar by a raw numeric value in place.
         */
        template <typename EB, typename = std::enable_if_t<is_numeric_v<EB>>>
        Domain<IS> &operator/=(const EB &_value);

        /**
         * @brief Applies the floating-point remainder with a raw numeric value in place.
         *
         * Only available when @p IS supports the modulo operation.
         */
        template <typename EB, typename = std::enable_if_t<MathTraits<IS>::supports_modulo_op && is_numeric_v<EB>>>
        Domain<IS> &operator%=(const EB &_value);

        // =====================================================================
        // Equality operators
        // =====================================================================

        /**
         * @brief Exact equality comparison with another domain scalar.
         */
        template <typename ES>
        [[nodiscard]] bool operator==(const Domain<ES> &_scalar) const;

        /**
         * @brief Exact inequality comparison with another domain scalar.
         */
        template <typename ES>
        [[nodiscard]] bool operator!=(const Domain<ES> &_scalar) const;

        /**
         * @brief Exact equality comparison with a raw numeric value.
         */
        template <typename EB, typename = std::enable_if_t<is_numeric_v<EB>>>
        [[nodiscard]] bool operator==(const EB &_value) const;

        /**
         * @brief Exact inequality comparison with a raw numeric value.
         */
        template <typename EB, typename = std::enable_if_t<is_numeric_v<EB>>>
        [[nodiscard]] bool operator!=(const EB &_value) const;

        // =====================================================================
        // Ordering operators — Domain<ES> operand
        // =====================================================================

        /**
         * @brief Less-than comparison.
         *
         * Only available when both @p IS and @p ES are ordered scalar types
         * (@c ScalarTraits<IS>::is_ordered and @c ScalarTraits<ES>::is_ordered).
         */
        template <typename ES, typename = std::enable_if_t<ScalarTraits<IS>::is_ordered && ScalarTraits<ES>::is_ordered>>
        [[nodiscard]] bool operator<(const Domain<ES> &_scalar) const;

        /**
         * @brief Less-than-or-equal comparison.
         *
         * Only available when both @p IS and @p ES are ordered scalar types
         * (@c ScalarTraits<IS>::is_ordered and @c ScalarTraits<ES>::is_ordered).
         */
        template <typename ES, typename = std::enable_if_t<ScalarTraits<IS>::is_ordered && ScalarTraits<ES>::is_ordered>>
        [[nodiscard]] bool operator<=(const Domain<ES> &_scalar) const;

        /**
         * @brief Greater-than comparison.
         *
         * Only available when both @p IS and @p ES are ordered scalar types
         * (@c ScalarTraits<IS>::is_ordered and @c ScalarTraits<ES>::is_ordered).
         */
        template <typename ES, typename = std::enable_if_t<ScalarTraits<IS>::is_ordered && ScalarTraits<ES>::is_ordered>>
        [[nodiscard]] bool operator>(const Domain<ES> &_scalar) const;

        /**
         * @brief Greater-than-or-equal comparison.
         *
         * Only available when both @p IS and @p ES are ordered scalar types
         * (@c ScalarTraits<IS>::is_ordered and @c ScalarTraits<ES>::is_ordered).
         */
        template <typename ES, typename = std::enable_if_t<ScalarTraits<IS>::is_ordered && ScalarTraits<ES>::is_ordered>>
        [[nodiscard]] bool operator>=(const Domain<ES> &_scalar) const;

        // =====================================================================
        // Ordering operators — raw numeric operand
        // =====================================================================

        /**
         * @brief Less-than comparison with a raw numeric value (ordered types only).
         *
         * Only available when @p IS is ordered scalar type.
         */
        template <typename EB, typename = std::enable_if_t<ScalarTraits<IS>::is_ordered && is_numeric_v<EB>>>
        [[nodiscard]] bool operator<(const EB &_value) const;

        /**
         * @brief Less-than-or-equal comparison with a raw numeric value (ordered types only).
         *
         * Only available when @p IS is ordered scalar type.
         */
        template <typename EB, typename = std::enable_if_t<ScalarTraits<IS>::is_ordered && is_numeric_v<EB>>>
        [[nodiscard]] bool operator<=(const EB &_value) const;

        /**
         * @brief Greater-than comparison with a raw numeric value (ordered types only).
         *
         * Only available when @p IS is ordered scalar type.
         */
        template <typename EB, typename = std::enable_if_t<ScalarTraits<IS>::is_ordered && is_numeric_v<EB>>>
        [[nodiscard]] bool operator>(const EB &_value) const;

        /**
         * @brief Greater-than-or-equal comparison with a raw numeric value (ordered types only).
         *
         * Only available when @p IS is ordered scalar type.
         */
        template <typename EB, typename = std::enable_if_t<ScalarTraits<IS>::is_ordered && is_numeric_v<EB>>>
        [[nodiscard]] bool operator>=(const EB &_value) const;

        // =====================================================================
        // Friend arithmetic operators (numeric op Domain — reversed order)
        // =====================================================================

        /**
         * @brief Adds a raw numeric value and a domain scalar (reversed order). @relates Domain
         */
        template <typename EB, typename = std::enable_if_t<is_numeric_v<EB>>>
        friend Domain<promote_t<IS, Real<to_float_t<EB>>>> operator+(const EB &_value, const Domain<IS> &_scalar)
        {
            using CS = promote_t<IS, Real<to_float_t<EB>>>;
            return Domain<CS>{_value + _scalar.inner_};
        }

        /**
         * @brief Subtracts a domain scalar from a raw numeric value (reversed order). @relates Domain
         */
        template <typename EB, typename = std::enable_if_t<is_numeric_v<EB>>>
        friend Domain<promote_t<IS, Real<to_float_t<EB>>>> operator-(const EB &_value, const Domain<IS> &_scalar)
        {
            using CS = promote_t<IS, Real<to_float_t<EB>>>;
            return Domain<CS>{_value - _scalar.inner_};
        }

        /**
         * @brief Multiplies a raw numeric value and a domain scalar (reversed order). @relates Domain
         */
        template <typename EB, typename = std::enable_if_t<is_numeric_v<EB>>>
        friend Domain<promote_t<IS, Real<to_float_t<EB>>>> operator*(const EB &_value, const Domain<IS> &_scalar)
        {
            using CS = promote_t<IS, Real<to_float_t<EB>>>;
            return Domain<CS>{_value * _scalar.inner_};
        }

        /**
         * @brief Divides a raw numeric value by a domain scalar (reversed order). @relates Domain
         */
        template <typename EB, typename = std::enable_if_t<is_numeric_v<EB>>>
        friend Domain<promote_t<IS, Real<to_float_t<EB>>>> operator/(const EB &_value, const Domain<IS> &_scalar)
        {
            using CS = promote_t<IS, Real<to_float_t<EB>>>;
            return Domain<CS>{_value / _scalar.inner_};
        }

        /**
         * @brief Computes the remainder of a raw numeric value divided by a domain scalar (reversed order).
         *
         * Only available when @p IS supports the modulo operation.
         *
         * @relates Domain
         */
        template <typename EB, typename = std::enable_if_t<MathTraits<IS>::supports_modulo_op && is_numeric_v<EB>>>
        friend Domain<promote_t<IS, Real<to_float_t<EB>>>> operator%(const EB &_value, const Domain<IS> &_scalar)
        {
            using CS = promote_t<IS, Real<to_float_t<EB>>>;
            return Domain<CS>{_value % _scalar.inner_};
        }

        // =====================================================================
        // Friend equality operators (numeric op Domain — reversed order)
        // =====================================================================

        /**
         * @brief Exact equality: raw numeric vs domain scalar (reversed order). @relates Domain
         */
        template <typename EB, typename = std::enable_if_t<is_numeric_v<EB>>>
        friend bool operator==(const EB &_value, const Domain<IS> &_scalar)
        {
            return _scalar == _value;
        }

        /**
         * @brief Exact inequality: raw numeric vs domain scalar (reversed order). @relates Domain
         */
        template <typename EB, typename = std::enable_if_t<is_numeric_v<EB>>>
        friend bool operator!=(const EB &_value, const Domain<IS> &_scalar)
        {
            return _scalar != _value;
        }

        // =====================================================================
        // Friend ordering operators (numeric op Domain — reversed order)
        // =====================================================================

        /**
         * @brief Less-than: raw numeric vs domain scalar (reversed order). @relates Domain
         */
        template <typename EB, typename = std::enable_if_t<ScalarTraits<IS>::is_ordered && is_numeric_v<EB>>>
        friend bool operator<(const EB &_value, const Domain<IS> &_scalar)
        {
            return _scalar > _value;
        }

        /**
         * @brief Less-than-or-equal: raw numeric vs domain scalar (reversed order). @relates Domain
         */
        template <typename EB, typename = std::enable_if_t<ScalarTraits<IS>::is_ordered && is_numeric_v<EB>>>
        friend bool operator<=(const EB &_value, const Domain<IS> &_scalar)
        {
            return _scalar >= _value;
        }

        /**
         * @brief Greater-than: raw numeric vs domain scalar (reversed order). @relates Domain
         */
        template <typename EB, typename = std::enable_if_t<ScalarTraits<IS>::is_ordered && is_numeric_v<EB>>>
        friend bool operator>(const EB &_value, const Domain<IS> &_scalar)
        {
            return _scalar < _value;
        }

        /**
         * @brief Greater-than-or-equal: raw numeric vs domain scalar (reversed order). @relates Domain
         */
        template <typename EB, typename = std::enable_if_t<ScalarTraits<IS>::is_ordered && is_numeric_v<EB>>>
        friend bool operator>=(const EB &_value, const Domain<IS> &_scalar)
        {
            return _scalar <= _value;
        }

        // =====================================================================
        // Stream output
        // =====================================================================

        /**
         * @brief Writes a human-readable representation of the domain scalar to a stream.
         *
         * Delegates to the inner scalar's @c to_string() implementation.
         *
         * @param _os     The output stream.
         * @param _scalar The domain scalar to write.
         * @return Reference to @p _os, for chaining.
         *
         * @relates Domain
         */
        friend std::ostream &operator<<(std::ostream &_os, const Domain<IS> &_scalar)
        {
            return _os << _scalar.inner_.to_string();
        }
    };

    // =========================================================================
    // Convenience aliases
    // =========================================================================

    /**
     * @brief Convenience alias for a real-valued domain scalar.
     *
     * @tparam B The underlying floating-point base type.
     *
     * @par Example
     * @code
     * RealScalar<double> frequency(440.0);
     * @endcode
     */
    template <typename B>
    using RealScalar = Domain<Real<B>>;

    /**
     * @brief Convenience alias for a complex-valued domain scalar.
     *
     * @tparam B The underlying floating-point base type.
     *
     * @par Example
     * @code
     * ComplexScalar<double> impedance(50.0, -10.0);
     * @endcode
     */
    template <typename B>
    using ComplexScalar = Domain<Complex<B>>;

} // namespace Scalar

#include "Domain.inl"
