/**
 * @file ScalarTypes.hpp
 * @brief Concrete scalar types: @ref Scalar::Real and @ref Scalar::Complex.
 *
 * This header defines the two concrete scalar classes that form the core of
 * the scalar type system:
 * - @ref Scalar::Real<IB>   — a thin, zero-overhead wrapper around a
 *   floating-point value, supporting arithmetic, comparison, and the full
 *   set of real-valued mathematical operations.
 * - @ref Scalar::Complex<IB> — a zero-overhead pair of floating-point values
 *   (real and imaginary components), supporting complex arithmetic, modulus,
 *   argument, and conjugate operations.
 *
 * Both classes inherit from @ref Scalar::ScalarBase via CRTP and satisfy the
 * layout guarantees verified by the @c static_assert checks at the bottom of
 * this file.
 *
 * @note Include this file (or the umbrella header) rather than individual
 *       component headers.
 */

#pragma once

#include "ScalarBase.hpp"

#include <cmath>
#include <stdexcept>
#include <string>
#include <type_traits>

namespace Scalar
{
    // =========================================================================
    // Real<IB>
    // =========================================================================

    /**
     * @brief Zero-overhead wrapper around a real floating-point value.
     *
     * @ref Real<IB> provides a strongly typed real scalar with a full set of
     * arithmetic, compound-assignment, and comparison operators, plus CRTP
     * hooks consumed by @ref ScalarBase. The underlying storage is a single
     * value of type @p IB with no padding or virtual dispatch.
     *
     * @tparam IB The underlying floating-point base type
     *            (@c float, @c double, or @c long @c double).
     *
     * @par Layout guarantee
     * @code
     * static_assert(sizeof(Real<IB>) == sizeof(IB));
     * @endcode
     */
    template <typename IB>
    class Real : public ScalarBase<Real<IB>>
    {
        static_assert(std::is_floating_point_v<IB>, "Real<IB>: IB must be float, double or long double.");

    private:
        IB value_; ///< The underlying scalar value.

    public:
        /// The underlying floating-point base type.
        using value_type = IB;

        // =====================================================================
        // Constructors
        // =====================================================================

        /**
         * @brief Default constructor. Initialises the value to zero.
         */
        constexpr Real() noexcept : value_(IB(0)) {}

        /**
         * @brief Constructs from any numeric scalar value.
         *
         * @tparam EB Source numeric type; must satisfy @ref is_numeric_v.
         * @param _value The value to wrap.
         */
        template <typename EB, typename = std::enable_if_t<is_numeric_v<EB>>>
        constexpr explicit Real(const EB &_value) noexcept;

        /**
         * @brief Converting constructor from a @ref Real with a different base type.
         *
         * @tparam EB Source base type; must be a floating-point type.
         * @param _real The source real scalar.
         */
        template <typename EB, typename = std::enable_if_t<std::is_floating_point_v<EB>>>
        constexpr explicit Real(const Real<EB> &_real) noexcept;

        // =====================================================================
        // Accessors
        // =====================================================================

        /**
         * @brief Returns the real value (const).
         * @return The underlying value of type @p IB.
         */
        [[nodiscard]] constexpr IB re() const noexcept;

        /**
         * @brief Returns a mutable reference to the real value.
         * @return Reference to the underlying value.
         */
        constexpr IB &re() noexcept;

        // =====================================================================
        // CRTP implementation hooks
        // =====================================================================

        /// @brief Returns the real part (identity for real scalars). @see ScalarBase::real_part
        [[nodiscard]] IB real_part_impl() const noexcept;

        /// @brief Returns the absolute value. @see ScalarBase::modulus
        [[nodiscard]] IB modulus_impl() const noexcept;

        /// @brief Returns @c true if the value is exactly zero. @see ScalarBase::is_zero
        [[nodiscard]] bool is_zero_impl() const noexcept;

        /// @brief Returns @c true if the value is NaN. @see ScalarBase::is_nan
        [[nodiscard]] bool is_nan_impl() const noexcept;

        /// @brief Returns a string representation of the value. @see ScalarBase::to_string
        [[nodiscard]] std::string to_string_impl() const;

        // =====================================================================
        // General methods
        // =====================================================================

        /**
         * @brief Tests whether the value is zero within a given tolerance.
         *
         * @tparam EB Tolerance type; must satisfy @ref is_numeric_v.
         * @param _tolerance Maximum absolute deviation from zero.
         * @return @c true if @c |value_| <= @p _tolerance.
         */
        template <typename EB, typename = std::enable_if_t<is_numeric_v<EB>>>
        [[nodiscard]] bool is_zero(const EB &_tolerance) const noexcept;

        // =====================================================================
        // Unary operators
        // =====================================================================

        /// @brief Unary plus. Returns a copy of this value.
        [[nodiscard]] Real operator+() const noexcept;

        /// @brief Unary minus. Returns the negation of this value.
        [[nodiscard]] Real operator-() const noexcept;

        // =====================================================================
        // Arithmetic operators — Real<EB> operand
        // =====================================================================

        /**
         * @brief Adds two real scalars, promoting to the common base type.
         */
        template <typename EB, typename = std::enable_if_t<std::is_floating_point_v<EB>>>
        [[nodiscard]] Real<common_float_t<IB, EB>> operator+(const Real<EB> &_real) const noexcept;

        /**
         * @brief Subtracts two real scalars, promoting to the common base type.
         */
        template <typename EB, typename = std::enable_if_t<std::is_floating_point_v<EB>>>
        [[nodiscard]] Real<common_float_t<IB, EB>> operator-(const Real<EB> &_real) const noexcept;

        /**
         * @brief Multiplies two real scalars, promoting to the common base type.
         */
        template <typename EB, typename = std::enable_if_t<std::is_floating_point_v<EB>>>
        [[nodiscard]] Real<common_float_t<IB, EB>> operator*(const Real<EB> &_real) const noexcept;

        /**
         * @brief Divides two real scalars, promoting to the common base type.
         */
        template <typename EB, typename = std::enable_if_t<std::is_floating_point_v<EB>>>
        [[nodiscard]] Real<common_float_t<IB, EB>> operator/(const Real<EB> &_real) const noexcept;

        /**
         * @brief Computes the floating-point remainder, promoting to the common base type.
         */
        template <typename EB, typename = std::enable_if_t<std::is_floating_point_v<EB>>>
        [[nodiscard]] Real<common_float_t<IB, EB>> operator%(const Real<EB> &_real) const noexcept;

        // =====================================================================
        // Arithmetic operators — raw numeric operand
        // =====================================================================

        /**
         * @brief Adds a raw numeric value to this scalar.
         */
        template <typename EB, typename = std::enable_if_t<is_numeric_v<EB>>>
        [[nodiscard]] Real<common_float_t<IB, EB>> operator+(const EB &_value) const noexcept;

        /**
         * @brief Subtracts a raw numeric value from this scalar.
         */
        template <typename EB, typename = std::enable_if_t<is_numeric_v<EB>>>
        [[nodiscard]] Real<common_float_t<IB, EB>> operator-(const EB &_value) const noexcept;

        /**
         * @brief Multiplies this scalar by a raw numeric value.
         */
        template <typename EB, typename = std::enable_if_t<is_numeric_v<EB>>>
        [[nodiscard]] Real<common_float_t<IB, EB>> operator*(const EB &_value) const noexcept;

        /**
         * @brief Divides this scalar by a raw numeric value.
         */
        template <typename EB, typename = std::enable_if_t<is_numeric_v<EB>>>
        [[nodiscard]] Real<common_float_t<IB, EB>> operator/(const EB &_value) const noexcept;

        /**
         * @brief Computes the floating-point remainder with a raw numeric value.
         */
        template <typename EB, typename = std::enable_if_t<is_numeric_v<EB>>>
        [[nodiscard]] Real<common_float_t<IB, EB>> operator%(const EB &_value) const noexcept;

        // =====================================================================
        // Compound assignment operators — Real<EB> operand
        // =====================================================================

        /**
         * @brief Adds @p _real to this value in place.
         */
        template <typename EB, typename = std::enable_if_t<std::is_floating_point_v<EB>>>
        Real<IB> &operator+=(const Real<EB> &_real) noexcept;

        /**
         * @brief Subtracts @p _real from this value in place.
         */
        template <typename EB, typename = std::enable_if_t<std::is_floating_point_v<EB>>>
        Real<IB> &operator-=(const Real<EB> &_real) noexcept;

        /**
         * @brief Multiplies this value by @p _real in place.
         */
        template <typename EB, typename = std::enable_if_t<std::is_floating_point_v<EB>>>
        Real<IB> &operator*=(const Real<EB> &_real) noexcept;

        /**
         * @brief Divides this value by @p _real in place.
         */
        template <typename EB, typename = std::enable_if_t<std::is_floating_point_v<EB>>>
        Real<IB> &operator/=(const Real<EB> &_real) noexcept;

        /**
         * @brief Applies floating-point remainder with @p _real in place.
         */
        template <typename EB, typename = std::enable_if_t<std::is_floating_point_v<EB>>>
        Real<IB> &operator%=(const Real<EB> &_real) noexcept;

        // =====================================================================
        // Compound assignment operators — raw numeric operand
        // =====================================================================

        /**
         * @brief Adds a raw numeric value to this scalar in place.
         */
        template <typename EB, typename = std::enable_if_t<is_numeric_v<EB>>>
        Real<IB> &operator+=(const EB &_value) noexcept;

        /**
         * @brief Subtracts a raw numeric value from this scalar in place.
         */
        template <typename EB, typename = std::enable_if_t<is_numeric_v<EB>>>
        Real<IB> &operator-=(const EB &_value) noexcept;

        /**
         * @brief Multiplies this scalar by a raw numeric value in place.
         */
        template <typename EB, typename = std::enable_if_t<is_numeric_v<EB>>>
        Real<IB> &operator*=(const EB &_value) noexcept;

        /**
         * @brief Divides this scalar by a raw numeric value in place.
         */
        template <typename EB, typename = std::enable_if_t<is_numeric_v<EB>>>
        Real<IB> &operator/=(const EB &_value) noexcept;

        /**
         * @brief Applies floating-point remainder with a raw numeric value in place.
         */
        template <typename EB, typename = std::enable_if_t<is_numeric_v<EB>>>
        Real<IB> &operator%=(const EB &_value) noexcept;

        // =====================================================================
        // Comparison operators — Real<EB> operand
        // =====================================================================

        /**
         * @brief Exact equality comparison with another @ref Real.
         */
        template <typename EB, typename = std::enable_if_t<std::is_floating_point_v<EB>>>
        [[nodiscard]] bool operator==(const Real<EB> &_real) const noexcept;

        /**
         * @brief Exact inequality comparison with another @ref Real.
         */
        template <typename EB, typename = std::enable_if_t<std::is_floating_point_v<EB>>>
        [[nodiscard]] bool operator!=(const Real<EB> &_real) const noexcept;

        /**
         * @brief Less-than comparison with another @ref Real.
         */
        template <typename EB, typename = std::enable_if_t<std::is_floating_point_v<EB>>>
        [[nodiscard]] bool operator<(const Real<EB> &_real) const noexcept;

        /**
         * @brief Less-than-or-equal comparison with another @ref Real.
         */
        template <typename EB, typename = std::enable_if_t<std::is_floating_point_v<EB>>>
        [[nodiscard]] bool operator<=(const Real<EB> &_real) const noexcept;

        /**
         * @brief Greater-than comparison with another @ref Real.
         */
        template <typename EB, typename = std::enable_if_t<std::is_floating_point_v<EB>>>
        [[nodiscard]] bool operator>(const Real<EB> &_real) const noexcept;

        /**
         * @brief Greater-than-or-equal comparison with another @ref Real.
         */
        template <typename EB, typename = std::enable_if_t<std::is_floating_point_v<EB>>>
        [[nodiscard]] bool operator>=(const Real<EB> &_real) const noexcept;

        // =====================================================================
        // Comparison operators — raw numeric operand
        // =====================================================================

        /**
         * @brief Exact equality comparison with a raw numeric value.
         */
        template <typename EB, typename = std::enable_if_t<is_numeric_v<EB>>>
        [[nodiscard]] bool operator==(const EB &_value) const noexcept;

        /**
         * @brief Exact inequality comparison with a raw numeric value.
         */
        template <typename EB, typename = std::enable_if_t<is_numeric_v<EB>>>
        [[nodiscard]] bool operator!=(const EB &_value) const noexcept;

        /**
         * @brief Less-than comparison with a raw numeric value.
         */
        template <typename EB, typename = std::enable_if_t<is_numeric_v<EB>>>
        [[nodiscard]] bool operator<(const EB &_value) const noexcept;

        /**
         * @brief Less-than-or-equal comparison with a raw numeric value.
         */
        template <typename EB, typename = std::enable_if_t<is_numeric_v<EB>>>
        [[nodiscard]] bool operator<=(const EB &_value) const noexcept;

        /**
         * @brief Greater-than comparison with a raw numeric value.
         */
        template <typename EB, typename = std::enable_if_t<is_numeric_v<EB>>>
        [[nodiscard]] bool operator>(const EB &_value) const noexcept;

        /**
         * @brief Greater-than-or-equal comparison with a raw numeric value.
         */
        template <typename EB, typename = std::enable_if_t<is_numeric_v<EB>>>
        [[nodiscard]] bool operator>=(const EB &_value) const noexcept;

        // =====================================================================
        // Friend arithmetic operators (scalar op Real — reversed operand order)
        // =====================================================================

        /**
         * @brief Adds a raw numeric value and a @ref Real (reversed order).
         * @relates Real
         */
        template <typename EB, typename = std::enable_if_t<is_numeric_v<EB>>>
        friend Real<common_float_t<IB, EB>> operator+(const EB &_value, const Real<IB> &_real) noexcept
        {
            using CB = common_float_t<IB, EB>;
            return Real<CB>(static_cast<CB>(_value)) + Real<CB>(_real);
        }

        /**
         * @brief Subtracts a @ref Real from a raw numeric value (reversed order).
         * @relates Real
         */
        template <typename EB, typename = std::enable_if_t<is_numeric_v<EB>>>
        friend Real<common_float_t<IB, EB>> operator-(const EB &_value, const Real<IB> &_real) noexcept
        {
            using CB = common_float_t<IB, EB>;
            return Real<CB>(static_cast<CB>(_value)) - Real<CB>(_real);
        }

        /**
         * @brief Multiplies a raw numeric value and a @ref Real (reversed order).
         * @relates Real
         */
        template <typename EB, typename = std::enable_if_t<is_numeric_v<EB>>>
        friend Real<common_float_t<IB, EB>> operator*(const EB &_value, const Real<IB> &_real) noexcept
        {
            using CB = common_float_t<IB, EB>;
            return Real<CB>(static_cast<CB>(_value)) * Real<CB>(_real);
        }

        /**
         * @brief Divides a raw numeric value by a @ref Real (reversed order).
         * @relates Real
         */
        template <typename EB, typename = std::enable_if_t<is_numeric_v<EB>>>
        friend Real<common_float_t<IB, EB>> operator/(const EB &_value, const Real<IB> &_real) noexcept
        {
            using CB = common_float_t<IB, EB>;
            return Real<CB>(static_cast<CB>(_value)) / Real<CB>(_real);
        }

        /**
         * @brief Computes the remainder of a raw numeric value divided by a @ref Real (reversed order).
         * @relates Real
         */
        template <typename EB, typename = std::enable_if_t<is_numeric_v<EB>>>
        friend Real<common_float_t<IB, EB>> operator%(const EB &_value, const Real<IB> &_real) noexcept
        {
            using CB = common_float_t<IB, EB>;
            return Real<CB>(static_cast<CB>(_value)) % Real<CB>(_real);
        }

        // =====================================================================
        // Friend comparison operators (scalar op Real — reversed operand order)
        // =====================================================================

        /**
         * @brief Exact equality: raw numeric vs @ref Real (reversed order). @relates Real
         */
        template <typename EB, typename = std::enable_if_t<is_numeric_v<EB>>>
        friend bool operator==(const EB &_value, const Real<IB> &_real) noexcept
        {
            using CB = common_float_t<IB, EB>;
            return static_cast<CB>(_value) == static_cast<CB>(_real.re());
        }

        /**
         * @brief Exact inequality: raw numeric vs @ref Real (reversed order). @relates Real
         */
        template <typename EB, typename = std::enable_if_t<is_numeric_v<EB>>>
        friend bool operator!=(const EB &_value, const Real<IB> &_real) noexcept
        {
            using CB = common_float_t<IB, EB>;
            return static_cast<CB>(_value) != static_cast<CB>(_real.re());
        }

        /**
         * @brief Less-than: raw numeric vs @ref Real (reversed order). @relates Real
         */
        template <typename EB, typename = std::enable_if_t<is_numeric_v<EB>>>
        friend bool operator<(const EB &_value, const Real<IB> &_real) noexcept
        {
            using CB = common_float_t<IB, EB>;
            return static_cast<CB>(_value) < static_cast<CB>(_real.re());
        }

        /**
         * @brief Less-than-or-equal: raw numeric vs @ref Real (reversed order). @relates Real
         */
        template <typename EB, typename = std::enable_if_t<is_numeric_v<EB>>>
        friend bool operator<=(const EB &_value, const Real<IB> &_real) noexcept
        {
            using CB = common_float_t<IB, EB>;
            return static_cast<CB>(_value) <= static_cast<CB>(_real.re());
        }

        /**
         * @brief Greater-than: raw numeric vs @ref Real (reversed order). @relates Real
         */
        template <typename EB, typename = std::enable_if_t<is_numeric_v<EB>>>
        friend bool operator>(const EB &_value, const Real<IB> &_real) noexcept
        {
            using CB = common_float_t<IB, EB>;
            return static_cast<CB>(_value) > static_cast<CB>(_real.re());
        }

        /**
         * @brief Greater-than-or-equal: raw numeric vs @ref Real (reversed order). @relates Real
         */
        template <typename EB, typename = std::enable_if_t<is_numeric_v<EB>>>
        friend bool operator>=(const EB &_value, const Real<IB> &_real) noexcept
        {
            using CB = common_float_t<IB, EB>;
            return static_cast<CB>(_value) >= static_cast<CB>(_real.re());
        }
    };

    // =========================================================================
    // Complex<IB>
    // =========================================================================

    /**
     * @brief Zero-overhead wrapper around a complex floating-point value.
     *
     * @ref Complex<IB> stores a real (@c re_) and imaginary (@c im_) component
     * of type @p IB. It provides arithmetic with other @ref Complex values,
     * with @ref Real values, and with raw numeric scalars, as well as the full
     * set of complex-specific operations (modulus, argument, conjugate).
     *
     * @tparam IB The underlying floating-point base type
     *            (@c float, @c double, or @c long @c double).
     *
     * @par Layout guarantee
     * @code
     * static_assert(sizeof(Complex<IB>) == 2 * sizeof(IB));
     * @endcode
     */
    template <typename IB>
    class Complex : public ScalarBase<Complex<IB>>
    {
        static_assert(std::is_floating_point_v<IB>,
                      "Complex<IB>: IB must be float, double or long double.");

    private:
        IB re_; ///< Real component.
        IB im_; ///< Imaginary component.

    public:
        /// The underlying floating-point base type.
        using value_type = IB;

        // =====================================================================
        // Constructors
        // =====================================================================

        /**
         * @brief Default constructor. Initialises both components to zero.
         */
        constexpr Complex() noexcept : re_(IB(0)), im_(IB(0)) {}

        /**
         * @brief Constructs from a single numeric value (real part only, imaginary = 0).
         *
         * @note Mark as @c explicit if implicit conversion from raw numerics is undesirable.
         *
         * @tparam EB Source numeric type; must satisfy @ref is_numeric_v.
         * @param _value Value assigned to the real component.
         */
        template <typename EB, typename = std::enable_if_t<is_numeric_v<EB>>>
        constexpr explicit Complex(const EB &_value) noexcept;

        /**
         * @brief Constructs from separate real and imaginary components.
         *
         * @tparam EB1 Type of the real component; must satisfy @ref is_numeric_v.
         * @tparam EB2 Type of the imaginary component; must satisfy @ref is_numeric_v.
         * @param _re_value Real component.
         * @param _im_value Imaginary component.
         */
        template <typename EB1, typename EB2, typename = std::enable_if_t<is_numeric_v<EB1> && is_numeric_v<EB2>>>
        constexpr Complex(const EB1 &_re_value, const EB2 &_im_value) noexcept;

        /**
         * @brief Constructs from a @ref Real scalar (imaginary part = 0).
         *
         * @tparam EB Base type of the source @ref Real; must be floating-point.
         * @param _real Source real scalar.
         */
        template <typename EB, typename = std::enable_if_t<std::is_floating_point_v<EB>>>
        constexpr explicit Complex(const Real<EB> &_real) noexcept;

        /**
         * @brief Converting constructor from a @ref Complex with a different base type.
         *
         * @tparam EB Source base type; must be floating-point.
         * @param _complex Source complex scalar.
         */
        template <typename EB, typename = std::enable_if_t<std::is_floating_point_v<EB>>>
        constexpr explicit Complex(const Complex<EB> &_complex) noexcept;

        // =====================================================================
        // Accessors
        // =====================================================================

        /**
         * @brief Returns the real component (const).
         */
        [[nodiscard]] constexpr IB re() const noexcept;

        /**
         * @brief Returns the imaginary component (const).
         */
        [[nodiscard]] constexpr IB im() const noexcept;

        /**
         * @brief Returns a mutable reference to the real component.
         */
        constexpr IB &re() noexcept;

        /**
         * @brief Returns a mutable reference to the imaginary component.
         */
        constexpr IB &im() noexcept;

        // =====================================================================
        // CRTP implementation hooks
        // =====================================================================

        /// @brief Returns the real component. @see ScalarBase::real_part
        [[nodiscard]] IB real_part_impl() const noexcept;

        /// @brief Returns the magnitude sqrt(re² + im²). @see ScalarBase::modulus
        [[nodiscard]] IB modulus_impl() const noexcept;

        /// @brief Returns the phase angle atan2(im, re). @see ScalarBase::argument
        [[nodiscard]] IB argument_impl() const noexcept;

        /// @brief Returns the complex conjugate (re - im·i). @see ScalarBase::conjugate
        [[nodiscard]] Complex conjugate_impl() const noexcept;

        /// @brief Returns @c true if both components are exactly zero. @see ScalarBase::is_zero
        [[nodiscard]] bool is_zero_impl() const noexcept;

        /// @brief Returns @c true if either component is NaN. @see ScalarBase::is_nan
        [[nodiscard]] bool is_nan_impl() const noexcept;

        /// @brief Returns a string representation (e.g. "3.0+2.0i"). @see ScalarBase::to_string
        [[nodiscard]] std::string to_string_impl() const;

        // =====================================================================
        // General methods
        // =====================================================================

        /**
         * @brief Tests whether the value is zero within a given tolerance.
         *
         * Returns @c true if the modulus is at most @p _tolerance.
         *
         * @tparam EB Tolerance type; must satisfy @ref is_numeric_v.
         * @param _tolerance Maximum acceptable modulus.
         * @return @c true if modulus() <= @p _tolerance.
         */
        template <typename EB, typename = std::enable_if_t<is_numeric_v<EB>>>
        [[nodiscard]] bool is_zero(const EB &_tolerance) const noexcept;

        // =====================================================================
        // Unary operators
        // =====================================================================

        /// @brief Unary plus. Returns a copy of this value.
        [[nodiscard]] Complex operator+() const noexcept;

        /// @brief Unary minus. Returns the negation of both components.
        [[nodiscard]] Complex operator-() const noexcept;

        // =====================================================================
        // Arithmetic operators — Complex<EB> operand
        // =====================================================================

        /**
         * @brief Adds two complex scalars, promoting to the common base type.
         */
        template <typename EB, typename = std::enable_if_t<std::is_floating_point_v<EB>>>
        [[nodiscard]] Complex<common_float_t<IB, EB>> operator+(const Complex<EB> &_complex) const noexcept;

        /**
         * @brief Subtracts two complex scalars, promoting to the common base type.
         */
        template <typename EB, typename = std::enable_if_t<std::is_floating_point_v<EB>>>
        [[nodiscard]] Complex<common_float_t<IB, EB>> operator-(const Complex<EB> &_complex) const noexcept;

        /**
         * @brief Multiplies two complex scalars, promoting to the common base type.
         */
        template <typename EB, typename = std::enable_if_t<std::is_floating_point_v<EB>>>
        [[nodiscard]] Complex<common_float_t<IB, EB>> operator*(const Complex<EB> &_complex) const noexcept;

        /**
         * @brief Divides two complex scalars, promoting to the common base type.
         */
        template <typename EB, typename = std::enable_if_t<std::is_floating_point_v<EB>>>
        [[nodiscard]] Complex<common_float_t<IB, EB>> operator/(const Complex<EB> &_complex) const noexcept;

        // =====================================================================
        // Arithmetic operators — Real<EB> operand
        // =====================================================================

        /**
         * @brief Adds a @ref Real to this complex scalar.
         */
        template <typename EB, typename = std::enable_if_t<std::is_floating_point_v<EB>>>
        [[nodiscard]] Complex<common_float_t<IB, EB>> operator+(const Real<EB> &_real) const noexcept;

        /**
         * @brief Subtracts a @ref Real from this complex scalar.
         */
        template <typename EB, typename = std::enable_if_t<std::is_floating_point_v<EB>>>
        [[nodiscard]] Complex<common_float_t<IB, EB>> operator-(const Real<EB> &_real) const noexcept;

        /**
         * @brief Multiplies this complex scalar by a @ref Real.
         */
        template <typename EB, typename = std::enable_if_t<std::is_floating_point_v<EB>>>
        [[nodiscard]] Complex<common_float_t<IB, EB>> operator*(const Real<EB> &_real) const noexcept;

        /**
         * @brief Divides this complex scalar by a @ref Real.
         */
        template <typename EB, typename = std::enable_if_t<std::is_floating_point_v<EB>>>
        [[nodiscard]] Complex<common_float_t<IB, EB>> operator/(const Real<EB> &_real) const noexcept;

        // =====================================================================
        // Arithmetic operators — raw numeric operand
        // =====================================================================

        /**
         * @brief Adds a raw numeric value to this complex scalar.
         */
        template <typename EB, typename = std::enable_if_t<is_numeric_v<EB>>>
        [[nodiscard]] Complex<common_float_t<IB, EB>> operator+(const EB &_value) const noexcept;

        /**
         * @brief Subtracts a raw numeric value from this complex scalar.
         */
        template <typename EB, typename = std::enable_if_t<is_numeric_v<EB>>>
        [[nodiscard]] Complex<common_float_t<IB, EB>> operator-(const EB &_value) const noexcept;

        /**
         * @brief Multiplies this complex scalar by a raw numeric value.
         */
        template <typename EB, typename = std::enable_if_t<is_numeric_v<EB>>>
        [[nodiscard]] Complex<common_float_t<IB, EB>> operator*(const EB &_value) const noexcept;

        /**
         * @brief Divides this complex scalar by a raw numeric value.
         */
        template <typename EB, typename = std::enable_if_t<is_numeric_v<EB>>>
        [[nodiscard]] Complex<common_float_t<IB, EB>> operator/(const EB &_value) const noexcept;

        // =====================================================================
        // Compound assignment operators — Complex<EB> operand
        // =====================================================================

        /**
         * @brief Adds @p _complex to this value in place.
         */
        template <typename EB, typename = std::enable_if_t<std::is_floating_point_v<EB>>>
        Complex<IB> &operator+=(const Complex<EB> &_complex) noexcept;

        /**
         * @brief Subtracts @p _complex from this value in place.
         */
        template <typename EB, typename = std::enable_if_t<std::is_floating_point_v<EB>>>
        Complex<IB> &operator-=(const Complex<EB> &_complex) noexcept;

        /**
         * @brief Multiplies this value by @p _complex in place.
         */
        template <typename EB, typename = std::enable_if_t<std::is_floating_point_v<EB>>>
        Complex<IB> &operator*=(const Complex<EB> &_complex) noexcept;

        /**
         * @brief Divides this value by @p _complex in place.
         */
        template <typename EB, typename = std::enable_if_t<std::is_floating_point_v<EB>>>
        Complex<IB> &operator/=(const Complex<EB> &_complex) noexcept;

        // =====================================================================
        // Compound assignment operators — Real<EB> operand
        // =====================================================================

        /**
         * @brief Adds a @ref Real to this value in place.
         */
        template <typename EB, typename = std::enable_if_t<std::is_floating_point_v<EB>>>
        Complex<IB> &operator+=(const Real<EB> &_real) noexcept;

        /**
         * @brief Subtracts a @ref Real from this value in place.
         */
        template <typename EB, typename = std::enable_if_t<std::is_floating_point_v<EB>>>
        Complex<IB> &operator-=(const Real<EB> &_real) noexcept;

        /**
         * @brief Multiplies this value by a @ref Real in place.
         */
        template <typename EB, typename = std::enable_if_t<std::is_floating_point_v<EB>>>
        Complex<IB> &operator*=(const Real<EB> &_real) noexcept;

        /**
         * @brief Divides this value by a @ref Real in place.
         */
        template <typename EB, typename = std::enable_if_t<std::is_floating_point_v<EB>>>
        Complex<IB> &operator/=(const Real<EB> &_real) noexcept;

        // =====================================================================
        // Compound assignment operators — raw numeric operand
        // =====================================================================

        /**
         * @brief Adds a raw numeric value to this scalar in place.
         */
        template <typename EB, typename = std::enable_if_t<is_numeric_v<EB>>>
        Complex<IB> &operator+=(const EB &_value) noexcept;

        /**
         * @brief Subtracts a raw numeric value from this scalar in place.
         */
        template <typename EB, typename = std::enable_if_t<is_numeric_v<EB>>>
        Complex<IB> &operator-=(const EB &_value) noexcept;

        /**
         * @brief Multiplies this scalar by a raw numeric value in place.
         */
        template <typename EB, typename = std::enable_if_t<is_numeric_v<EB>>>
        Complex<IB> &operator*=(const EB &_value) noexcept;

        /**
         * @brief Divides this scalar by a raw numeric value in place.
         */
        template <typename EB, typename = std::enable_if_t<is_numeric_v<EB>>>
        Complex<IB> &operator/=(const EB &_value) noexcept;

        // =====================================================================
        // Equality operators — Complex<EB> operand
        // =====================================================================

        /**
         * @brief Exact equality comparison with another @ref Complex.
         */
        template <typename EB, typename = std::enable_if_t<std::is_floating_point_v<EB>>>
        [[nodiscard]] bool operator==(const Complex<EB> &_complex) const noexcept;

        /**
         * @brief Exact inequality comparison with another @ref Complex.
         */
        template <typename EB, typename = std::enable_if_t<std::is_floating_point_v<EB>>>
        [[nodiscard]] bool operator!=(const Complex<EB> &_complex) const noexcept;

        // =====================================================================
        // Equality operators — Real<EB> operand
        // =====================================================================

        /**
         * @brief Exact equality with a @ref Real (imaginary part must be zero).
         *
         * Returns @c true only if the real components are equal and the imaginary
         * component of this value is exactly zero.
         */
        template <typename EB, typename = std::enable_if_t<std::is_floating_point_v<EB>>>
        [[nodiscard]] bool operator==(const Real<EB> &_real) const noexcept;

        /**
         * @brief Exact inequality with a @ref Real.
         */
        template <typename EB, typename = std::enable_if_t<std::is_floating_point_v<EB>>>
        [[nodiscard]] bool operator!=(const Real<EB> &_real) const noexcept;

        // =====================================================================
        // Equality operators — raw numeric operand
        // =====================================================================

        /**
         * @brief Exact equality with a raw numeric value (imaginary part must be zero).
         *
         * Returns @c true only if the real component equals @p _value and the
         * imaginary component is exactly zero.
         */
        template <typename EB, typename = std::enable_if_t<is_numeric_v<EB>>>
        [[nodiscard]] bool operator==(const EB &_value) const noexcept;

        /**
         * @brief Exact inequality with a raw numeric value.
         */
        template <typename EB, typename = std::enable_if_t<is_numeric_v<EB>>>
        [[nodiscard]] bool operator!=(const EB &_value) const noexcept;

        // =====================================================================
        // Friend arithmetic operators (scalar/Real op Complex — reversed order)
        // =====================================================================

        /**
         * @brief Adds a raw numeric value and a @ref Complex (reversed order). @relates Complex
         */
        template <typename EB, typename = std::enable_if_t<is_numeric_v<EB>>>
        friend Complex<common_float_t<IB, EB>> operator+(const EB &_value, const Complex<IB> &_complex) noexcept
        {
            using CB = common_float_t<IB, EB>;
            return Complex<CB>(static_cast<CB>(_value), CB(0)) + Complex<CB>(_complex);
        }

        /**
         * @brief Subtracts a @ref Complex from a raw numeric value (reversed order). @relates Complex
         */
        template <typename EB, typename = std::enable_if_t<is_numeric_v<EB>>>
        friend Complex<common_float_t<IB, EB>> operator-(const EB &_value, const Complex<IB> &_complex) noexcept
        {
            using CB = common_float_t<IB, EB>;
            return Complex<CB>(static_cast<CB>(_value), CB(0)) - Complex<CB>(_complex);
        }

        /**
         * @brief Multiplies a raw numeric value and a @ref Complex (reversed order). @relates Complex
         */
        template <typename EB, typename = std::enable_if_t<is_numeric_v<EB>>>
        friend Complex<common_float_t<IB, EB>> operator*(const EB &_value, const Complex<IB> &_complex) noexcept
        {
            using CB = common_float_t<IB, EB>;
            return Complex<CB>(static_cast<CB>(_value), CB(0)) * Complex<CB>(_complex);
        }

        /**
         * @brief Divides a raw numeric value by a @ref Complex (reversed order). @relates Complex
         */
        template <typename EB, typename = std::enable_if_t<is_numeric_v<EB>>>
        friend Complex<common_float_t<IB, EB>> operator/(const EB &_value, const Complex<IB> &_complex) noexcept
        {
            using CB = common_float_t<IB, EB>;
            return Complex<CB>(static_cast<CB>(_value), CB(0)) / Complex<CB>(_complex);
        }

        /**
         * @brief Adds a @ref Real and a @ref Complex (reversed order). @relates Complex
         */
        template <typename EB, typename = std::enable_if_t<std::is_floating_point_v<EB>>>
        friend Complex<common_float_t<IB, EB>> operator+(const Real<EB> &_real, const Complex<IB> &_complex) noexcept
        {
            using CB = common_float_t<IB, EB>;
            return Complex<CB>(static_cast<CB>(_real.re()), CB(0)) + Complex<CB>(_complex);
        }

        /**
         * @brief Subtracts a @ref Complex from a @ref Real (reversed order). @relates Complex
         */
        template <typename EB, typename = std::enable_if_t<std::is_floating_point_v<EB>>>
        friend Complex<common_float_t<IB, EB>> operator-(const Real<EB> &_real, const Complex<IB> &_complex) noexcept
        {
            using CB = common_float_t<IB, EB>;
            return Complex<CB>(static_cast<CB>(_real.re()), CB(0)) - Complex<CB>(_complex);
        }

        /**
         * @brief Multiplies a @ref Real and a @ref Complex (reversed order). @relates Complex
         */
        template <typename EB, typename = std::enable_if_t<std::is_floating_point_v<EB>>>
        friend Complex<common_float_t<IB, EB>> operator*(const Real<EB> &_real, const Complex<IB> &_complex) noexcept
        {
            using CB = common_float_t<IB, EB>;
            return Complex<CB>(static_cast<CB>(_real.re()), CB(0)) * Complex<CB>(_complex);
        }

        /**
         * @brief Divides a @ref Real by a @ref Complex (reversed order). @relates Complex
         */
        template <typename EB, typename = std::enable_if_t<std::is_floating_point_v<EB>>>
        friend Complex<common_float_t<IB, EB>> operator/(const Real<EB> &_real, const Complex<IB> &_complex) noexcept
        {
            using CB = common_float_t<IB, EB>;
            return Complex<CB>(static_cast<CB>(_real.re()), CB(0)) / Complex<CB>(_complex);
        }

        // =====================================================================
        // Friend equality operators (scalar/Real op Complex — reversed order)
        // =====================================================================

        /**
         * @brief Exact equality: raw numeric vs @ref Complex (reversed order).
         *
         * Returns @c true only if the real component equals @p _value and the
         * imaginary component of @p _complex is exactly zero.
         *
         * @relates Complex
         */
        template <typename EB, typename = std::enable_if_t<is_numeric_v<EB>>>
        friend bool operator==(const EB &_value, const Complex<IB> &_complex) noexcept
        {
            using CB = common_float_t<IB, EB>;
            return static_cast<CB>(_value) == static_cast<CB>(_complex.re()) && CB(0) == static_cast<CB>(_complex.im());
        }

        /**
         * @brief Exact inequality: raw numeric vs @ref Complex (reversed order). @relates Complex
         */
        template <typename EB, typename = std::enable_if_t<is_numeric_v<EB>>>
        friend bool operator!=(const EB &_value, const Complex<IB> &_complex) noexcept
        {
            return !(_value == _complex);
        }

        /**
         * @brief Exact equality: @ref Real vs @ref Complex (reversed order).
         *
         * Returns @c true only if the real components are equal and the imaginary
         * component of @p _complex is exactly zero.
         *
         * @relates Complex
         */
        template <typename EB, typename = std::enable_if_t<std::is_floating_point_v<EB>>>
        friend bool operator==(const Real<EB> &_real, const Complex<IB> &_complex) noexcept
        {
            using CB = common_float_t<IB, EB>;
            return static_cast<CB>(_real.re()) == static_cast<CB>(_complex.re()) && CB(0) == static_cast<CB>(_complex.im());
        }

        /**
         * @brief Exact inequality: @ref Real vs @ref Complex (reversed order). @relates Complex
         */
        template <typename EB, typename = std::enable_if_t<std::is_floating_point_v<EB>>>
        friend bool operator!=(const Real<EB> &_real, const Complex<IB> &_complex) noexcept
        {
            return !(_real == _complex);
        }
    };

    // =========================================================================
    // Zero-overhead layout guarantees
    // =========================================================================

    static_assert(sizeof(Real<float>) == sizeof(float), "Real<float> must have no overhead");
    static_assert(sizeof(Real<double>) == sizeof(double), "Real<double> must have no overhead");
    static_assert(sizeof(Real<long double>) == sizeof(long double), "Real<long double> must have no overhead");
    static_assert(sizeof(Complex<float>) == 2 * sizeof(float), "Complex<float> must have no overhead");
    static_assert(sizeof(Complex<double>) == 2 * sizeof(double), "Complex<double> must have no overhead");
    static_assert(sizeof(Complex<long double>) == 2 * sizeof(long double), "Complex<long double> must have no overhead");

} // namespace Scalar

#include "ScalarTraits.inl"
#include "ScalarTypes.inl"
