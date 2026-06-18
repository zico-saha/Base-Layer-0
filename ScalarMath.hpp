/**
 * @file ScalarMath.hpp
 * @brief Free-function mathematical operations over @ref Scalar::Domain scalars.
 *
 * This header declares the public mathematical API for the scalar library:
 * power, root, exponential, logarithm, rounding, clamping, trigonometric,
 * hyperbolic, and complex-specific functions (modulus, argument, conjugate,
 * polar construction), all operating on @ref Scalar::Domain<S> values.
 *
 * Each public function is gated via @ref Scalar::MathTraits or
 * @ref Scalar::ScalarTraits so that operations unsupported by a given scalar
 * type (e.g. @c floor on a @ref Scalar::Complex domain) fail to compile rather
 * than producing undefined or nonsensical results.
 *
 * Functions suffixed with @c _c (e.g. @ref sqrt_c, @ref ln_c) accept a
 * @ref Scalar::Real domain but always return a @ref Scalar::Complex domain,
 * covering cases where the real-valued result may not exist (e.g. the square
 * root of a negative real).
 *
 * @ref Scalar::Math::Detail contains internal implementation functions and is
 * not part of the public API; callers should use the functions declared
 * directly in @ref Scalar::Math.
 *
 * @note Include this file (or the umbrella header) rather than individual
 *       component headers.
 */

#pragma once

#include "Domain.hpp"

#include <cmath>
#include <numbers>
#include <type_traits>

namespace Scalar::Math
{
    /**
     * @namespace Scalar::Math::Detail
     * @brief Internal implementation functions for @ref Scalar::Math.
     *
     * These overloads operate directly on @ref Scalar::Real and
     * @ref Scalar::Complex (not @ref Scalar::Domain) and perform the actual
     * numerical computation dispatched to by the public API. Not intended for
     * direct use by client code.
     */
    namespace Detail
    {
        /**
         * @brief Computes @p _base raised to @p _exponent for real operands.
         */
        template <typename B, typename = std::enable_if_t<std::is_floating_point_v<B>>>
        Real<B> pow_impl(const Real<B> &_base, const Real<B> &_exponent);

        /**
         * @brief Computes @p _base raised to @p _exponent for complex operands.
         */
        template <typename B, typename = std::enable_if_t<std::is_floating_point_v<B>>>
        Complex<B> pow_impl(const Complex<B> &_base, const Complex<B> &_exponent);

        /**
         * @brief Computes @p _base raised to an integral @p _exponent via fast exponentiation.
         *
         * @tparam S Scalar type; must satisfy @c MathTraits<S>::supports_pow.
         * @tparam B Integral exponent type.
         */
        template <typename S, typename B, typename = std::enable_if_t<MathTraits<S>::supports_pow && std::is_integral_v<B>>>
        S pow_int_impl(const S &_base, const B &_exponent);

        /**
         * @brief Computes the square root of a real radicand.
         */
        template <typename B, typename = std::enable_if_t<std::is_floating_point_v<B>>>
        Real<B> sqrt_impl(const Real<B> &_radicand);

        /**
         * @brief Computes the principal square root of a complex radicand.
         */
        template <typename B, typename = std::enable_if_t<std::is_floating_point_v<B>>>
        Complex<B> sqrt_impl(const Complex<B> &_radicand);

        /**
         * @brief Computes the cube root of a real radicand.
         */
        template <typename B, typename = std::enable_if_t<std::is_floating_point_v<B>>>
        Real<B> cbrt_impl(const Real<B> &_radicand);

        /**
         * @brief Computes the principal cube root of a complex radicand.
         */
        template <typename B, typename = std::enable_if_t<std::is_floating_point_v<B>>>
        Complex<B> cbrt_impl(const Complex<B> &_radicand);

        /**
         * @brief Computes the exponential function for a real exponent.
         */
        template <typename B, typename = std::enable_if_t<std::is_floating_point_v<B>>>
        Real<B> exp_impl(const Real<B> &_exponent);

        /**
         * @brief Computes the exponential function for a complex exponent.
         */
        template <typename B, typename = std::enable_if_t<std::is_floating_point_v<B>>>
        Complex<B> exp_impl(const Complex<B> &_exponent);

        /**
         * @brief Computes the natural logarithm of a real argument.
         */
        template <typename B, typename = std::enable_if_t<std::is_floating_point_v<B>>>
        Real<B> ln_impl(const Real<B> &_argument);

        /**
         * @brief Computes the principal natural logarithm of a complex argument.
         */
        template <typename B, typename = std::enable_if_t<std::is_floating_point_v<B>>>
        Complex<B> ln_impl(const Complex<B> &_argument);

        /**
         * @brief Rounds a real value to a given number of decimal places.
         * @param _value          The value to round.
         * @param _decimal_places Number of decimal places to round to (default 0).
         */
        template <typename B, typename = std::enable_if_t<std::is_floating_point_v<B>>>
        Real<B> round_impl(const Real<B> &_value, const int &_decimal_places = 0);

        /**
         * @brief Rounds a complex value's components to a given number of decimal places.
         * @param _value          The value to round.
         * @param _decimal_places Number of decimal places to round to (default 0).
         */
        template <typename B, typename = std::enable_if_t<std::is_floating_point_v<B>>>
        Complex<B> round_impl(const Complex<B> &_value, const int &_decimal_places = 0);

        /**
         * @brief Computes the sine of a real angle.
         */
        template <typename B, typename = std::enable_if_t<std::is_floating_point_v<B>>>
        Real<B> sin_impl(const Real<B> &_angle);

        /**
         * @brief Computes the sine of a complex angle.
         */
        template <typename B, typename = std::enable_if_t<std::is_floating_point_v<B>>>
        Complex<B> sin_impl(const Complex<B> &_angle);

        /**
         * @brief Computes the cosine of a real angle.
         */
        template <typename B, typename = std::enable_if_t<std::is_floating_point_v<B>>>
        Real<B> cos_impl(const Real<B> &_angle);

        /**
         * @brief Computes the cosine of a complex angle.
         */
        template <typename B, typename = std::enable_if_t<std::is_floating_point_v<B>>>
        Complex<B> cos_impl(const Complex<B> &_angle);

        /**
         * @brief Computes the arcsine of a real value.
         */
        template <typename B, typename = std::enable_if_t<std::is_floating_point_v<B>>>
        Real<B> asin_impl(const Real<B> &_value);

        /**
         * @brief Computes the principal arcsine of a complex value.
         */
        template <typename B, typename = std::enable_if_t<std::is_floating_point_v<B>>>
        Complex<B> asin_impl(const Complex<B> &_value);

        /**
         * @brief Computes the arccosine of a real value.
         */
        template <typename B, typename = std::enable_if_t<std::is_floating_point_v<B>>>
        Real<B> acos_impl(const Real<B> &_value);

        /**
         * @brief Computes the principal arccosine of a complex value.
         */
        template <typename B, typename = std::enable_if_t<std::is_floating_point_v<B>>>
        Complex<B> acos_impl(const Complex<B> &_value);

        /**
         * @brief Computes the arctangent of a real value.
         */
        template <typename B, typename = std::enable_if_t<std::is_floating_point_v<B>>>
        Real<B> atan_impl(const Real<B> &_value);

        /**
         * @brief Computes the principal arctangent of a complex value.
         */
        template <typename B, typename = std::enable_if_t<std::is_floating_point_v<B>>>
        Complex<B> atan_impl(const Complex<B> &_value);

        /**
         * @brief Computes the hyperbolic sine of a real value.
         */
        template <typename B, typename = std::enable_if_t<std::is_floating_point_v<B>>>
        Real<B> sinh_impl(const Real<B> &_value);

        /**
         * @brief Computes the hyperbolic sine of a complex value.
         */
        template <typename B, typename = std::enable_if_t<std::is_floating_point_v<B>>>
        Complex<B> sinh_impl(const Complex<B> &_value);

        /**
         * @brief Computes the hyperbolic cosine of a real value.
         */
        template <typename B, typename = std::enable_if_t<std::is_floating_point_v<B>>>
        Real<B> cosh_impl(const Real<B> &_value);

        /**
         * @brief Computes the hyperbolic cosine of a complex value.
         */
        template <typename B, typename = std::enable_if_t<std::is_floating_point_v<B>>>
        Complex<B> cosh_impl(const Complex<B> &_value);

        /**
         * @brief Computes the inverse hyperbolic sine of a real value.
         */
        template <typename B, typename = std::enable_if_t<std::is_floating_point_v<B>>>
        Real<B> asinh_impl(const Real<B> &_value);

        /**
         * @brief Computes the principal inverse hyperbolic sine of a complex value.
         */
        template <typename B, typename = std::enable_if_t<std::is_floating_point_v<B>>>
        Complex<B> asinh_impl(const Complex<B> &_value);

        /**
         * @brief Computes the inverse hyperbolic cosine of a real value.
         */
        template <typename B, typename = std::enable_if_t<std::is_floating_point_v<B>>>
        Real<B> acosh_impl(const Real<B> &_value);

        /**
         * @brief Computes the principal inverse hyperbolic cosine of a complex value.
         */
        template <typename B, typename = std::enable_if_t<std::is_floating_point_v<B>>>
        Complex<B> acosh_impl(const Complex<B> &_value);

        /**
         * @brief Computes the inverse hyperbolic tangent of a real value.
         */
        template <typename B, typename = std::enable_if_t<std::is_floating_point_v<B>>>
        Real<B> atanh_impl(const Real<B> &_value);

        /**
         * @brief Computes the principal inverse hyperbolic tangent of a complex value.
         */
        template <typename B, typename = std::enable_if_t<std::is_floating_point_v<B>>>
        Complex<B> atanh_impl(const Complex<B> &_value);

        /**
         * @brief Returns the argument (sign-based angle: 0 or pi) of a real value.
         */
        template <typename B, typename = std::enable_if_t<std::is_floating_point_v<B>>>
        Real<B> argument_impl(const Real<B> &_value);

        /**
         * @brief Returns the argument (phase angle) of a complex value.
         */
        template <typename B, typename = std::enable_if_t<std::is_floating_point_v<B>>>
        Real<B> argument_impl(const Complex<B> &_value);

        /**
         * @brief Returns the norm (squared modulus) of a real value.
         */
        template <typename B, typename = std::enable_if_t<std::is_floating_point_v<B>>>
        Real<B> norm_impl(const Real<B> &_x);

        /**
         * @brief Returns the norm (squared modulus) of a complex value.
         */
        template <typename B, typename = std::enable_if_t<std::is_floating_point_v<B>>>
        Real<B> norm_impl(const Complex<B> &_x);

    } // namespace Detail

    // =========================================================================
    // Power functions
    // =========================================================================

    /**
     * @brief Raises @p _base to the power of @p _exponent, both domain scalars.
     *
     * @tparam S1 Base scalar type; must satisfy @c MathTraits<S1>::supports_pow.
     * @tparam S2 Exponent scalar type; must satisfy @c MathTraits<S2>::supports_pow.
     * @param _base     The base value.
     * @param _exponent The exponent value.
     * @return @p _base raised to @p _exponent, in the promoted scalar type.
     */
    template <typename S1, typename S2, typename = std::enable_if_t<MathTraits<S1>::supports_pow && MathTraits<S2>::supports_pow>>
    [[nodiscard]] Domain<promote_t<S1, S2>> pow(const Domain<S1> &_base, const Domain<S2> &_exponent);

    /**
     * @brief Raises a domain scalar @p _base to a raw floating-point @p _exponent.
     *
     * @tparam S Base scalar type; must satisfy @c MathTraits<S>::supports_pow.
     * @tparam B Exponent type; must be floating-point.
     */
    template <typename S, typename B, typename = std::enable_if_t<MathTraits<S>::supports_pow && std::is_floating_point_v<B>>>
    [[nodiscard]] Domain<promote_t<S, Real<B>>> pow(const Domain<S> &_base, const B &_exponent);

    /**
     * @brief Raises a domain scalar @p _base to an integral @p _exponent.
     *
     * Uses fast (repeated-squaring) exponentiation and preserves the base's
     * scalar type in the result (no promotion to a floating-point exponent type).
     *
     * @tparam S Base scalar type; must satisfy @c MathTraits<S>::supports_pow.
     * @tparam B Exponent type; must be integral.
     */
    template <typename S, typename B, typename = std::enable_if_t<MathTraits<S>::supports_pow && std::is_integral_v<B>>>
    [[nodiscard]] Domain<S> pow(const Domain<S> &_base, const B &_exponent);

    /**
     * @brief Raises a raw numeric @p _base to a domain scalar @p _exponent.
     *
     * @tparam S Exponent scalar type; must satisfy @c MathTraits<S>::supports_pow.
     * @tparam B Base type; must satisfy @ref is_numeric_v.
     */
    template <typename S, typename B, typename = std::enable_if_t<MathTraits<S>::supports_pow && is_numeric_v<B>>>
    [[nodiscard]] Domain<promote_t<S, Real<to_float_t<B>>>> pow(const B &_base, const Domain<S> &_exponent);

    // =========================================================================
    // Root functions
    // =========================================================================

    /**
     * @brief Computes the square root of a domain scalar.
     * @tparam S Scalar type; must satisfy @c MathTraits<S>::supports_sqrt.
     */
    template <typename S, typename = std::enable_if_t<MathTraits<S>::supports_sqrt>>
    [[nodiscard]] Domain<S> sqrt(const Domain<S> &_radicand);

    /**
     * @brief Computes the cube root of a domain scalar.
     * @tparam S Scalar type; must satisfy @c MathTraits<S>::supports_sqrt.
     */
    template <typename S, typename = std::enable_if_t<MathTraits<S>::supports_sqrt>>
    [[nodiscard]] Domain<S> cbrt(const Domain<S> &_radicand);

    // =========================================================================
    // Exponential functions
    // =========================================================================

    /**
     * @brief Computes the natural exponential (e^x) of a domain scalar.
     * @tparam S Scalar type; must satisfy @c MathTraits<S>::supports_exp.
     */
    template <typename S, typename = std::enable_if_t<MathTraits<S>::supports_exp>>
    [[nodiscard]] Domain<S> exp(const Domain<S> &_exponent);

    /**
     * @brief Computes the base-2 exponential (2^x) of a domain scalar.
     * @tparam S Scalar type; must satisfy @c MathTraits<S>::supports_exp.
     */
    template <typename S, typename = std::enable_if_t<MathTraits<S>::supports_exp>>
    [[nodiscard]] Domain<S> exp2(const Domain<S> &_exponent);

    // =========================================================================
    // Logarithm functions
    // =========================================================================

    /**
     * @brief Computes the logarithm of @p _argument with respect to @p _base, both domain scalars.
     *
     * @tparam S1 Argument scalar type; must satisfy @c MathTraits<S1>::supports_log.
     * @tparam S2 Base scalar type; must satisfy @c MathTraits<S2>::supports_log.
     */
    template <typename S1, typename S2, typename = std::enable_if_t<MathTraits<S1>::supports_log && MathTraits<S2>::supports_log>>
    [[nodiscard]] Domain<promote_t<S1, S2>> log(const Domain<S1> &_argument, const Domain<S2> &_base);

    /**
     * @brief Computes the logarithm of a domain scalar @p _argument with a raw numeric @p _base.
     * @tparam S Argument scalar type; must satisfy @c MathTraits<S>::supports_log.
     * @tparam B Base type; must satisfy @ref is_numeric_v.
     */
    template <typename S, typename B, typename = std::enable_if_t<MathTraits<S>::supports_log && is_numeric_v<B>>>
    [[nodiscard]] Domain<promote_t<S, Real<to_float_t<B>>>> log(const Domain<S> &_argument, const B &_base);

    /**
     * @brief Computes the logarithm of a raw numeric @p _argument with a domain scalar @p _base.
     * @tparam S Base scalar type; must satisfy @c MathTraits<S>::supports_log.
     * @tparam B Argument type; must satisfy @ref is_numeric_v.
     */
    template <typename S, typename B, typename = std::enable_if_t<MathTraits<S>::supports_log && is_numeric_v<B>>>
    [[nodiscard]] Domain<promote_t<S, Real<to_float_t<B>>>> log(const B &_argument, const Domain<S> &_base);

    /**
     * @brief Computes the natural logarithm of a domain scalar.
     * @tparam S Scalar type; must satisfy @c MathTraits<S>::supports_log.
     */
    template <typename S, typename = std::enable_if_t<MathTraits<S>::supports_log>>
    [[nodiscard]] Domain<S> ln(const Domain<S> &_argument);

    /**
     * @brief Computes the base-2 logarithm of a domain scalar.
     * @tparam S Scalar type; must satisfy @c MathTraits<S>::supports_log.
     */
    template <typename S, typename = std::enable_if_t<MathTraits<S>::supports_log>>
    [[nodiscard]] Domain<S> log2(const Domain<S> &_argument);

    /**
     * @brief Computes the base-10 logarithm of a domain scalar.
     * @tparam S Scalar type; must satisfy @c MathTraits<S>::supports_log.
     */
    template <typename S, typename = std::enable_if_t<MathTraits<S>::supports_log>>
    [[nodiscard]] Domain<S> log10(const Domain<S> &_argument);

    // =========================================================================
    // Modulus and sign functions
    // =========================================================================

    /**
     * @brief Computes the absolute value (modulus) of a domain scalar.
     * @tparam S Scalar type; must satisfy @c MathTraits<S>::supports_modulus.
     * @return A real-valued domain holding the modulus.
     */
    template <typename S, typename = std::enable_if_t<MathTraits<S>::supports_modulus>>
    [[nodiscard]] Domain<Real<typename ScalarTraits<S>::base_type>> abs(const Domain<S> &_value);

    /**
     * @brief Computes the sign of a domain scalar (-1, 0, or 1).
     * @tparam S Scalar type; must be ordered (@c ScalarTraits<S>::is_ordered).
     */
    template <typename S, typename = std::enable_if_t<ScalarTraits<S>::is_ordered>>
    [[nodiscard]] Domain<S> sign(const Domain<S> &_value);

    // =========================================================================
    // Floor, ceiling, and rounding functions
    // =========================================================================

    /**
     * @brief Rounds a domain scalar down to the nearest integer.
     * @tparam S Scalar type; must satisfy @c MathTraits<S>::supports_floor.
     */
    template <typename S, typename = std::enable_if_t<MathTraits<S>::supports_floor>>
    [[nodiscard]] Domain<S> floor(const Domain<S> &_value);

    /**
     * @brief Rounds a domain scalar up to the nearest integer.
     * @tparam S Scalar type; must satisfy @c MathTraits<S>::supports_ceil.
     */
    template <typename S, typename = std::enable_if_t<MathTraits<S>::supports_ceil>>
    [[nodiscard]] Domain<S> ceil(const Domain<S> &_value);

    /**
     * @brief Rounds a domain scalar to a given number of decimal places.
     * @tparam S Scalar type; must satisfy @c MathTraits<S>::supports_round.
     * @param _value          The value to round.
     * @param _decimal_places Number of decimal places to round to (default 0).
     */
    template <typename S, typename = std::enable_if_t<MathTraits<S>::supports_round>>
    [[nodiscard]] Domain<S> round(const Domain<S> &_value, const int &_decimal_places = 0);

    // =========================================================================
    // Clamp functions
    // =========================================================================

    /**
     * @brief Clamps a domain scalar @p _value between @p _min_value and @p _max_value, all domain scalars.
     *
     * @tparam S1 Type of @p _value; must satisfy @c MathTraits<S1>::supports_clip.
     * @tparam S2 Type of @p _min_value; must satisfy @c MathTraits<S2>::supports_clip.
     * @tparam S3 Type of @p _max_value; must satisfy @c MathTraits<S3>::supports_clip.
     * @return The clamped value, with the type of @p _value.
     */
    template <typename S1, typename S2, typename S3, typename = std::enable_if_t<MathTraits<S1>::supports_clip && MathTraits<S2>::supports_clip && MathTraits<S3>::supports_clip>>
    [[nodiscard]] Domain<S1> clamp(const Domain<S1> &_value, const Domain<S2> &_min_value, const Domain<S3> &_max_value);

    /**
     * @brief Clamps a domain scalar @p _value between a domain @p _min_value and a raw numeric @p _max_value.
     *
     * @tparam S1 Type of @p _value; must satisfy @c MathTraits<S1>::supports_clip.
     * @tparam S2 Type of @p _min_value; must satisfy @c MathTraits<S2>::supports_clip.
     * @tparam B  Type of @p _max_value; must satisfy @ref is_numeric_v.
     */
    template <typename S1, typename S2, typename B, typename = std::enable_if_t<MathTraits<S1>::supports_clip && MathTraits<S2>::supports_clip && is_numeric_v<B>>>
    [[nodiscard]] Domain<S1> clamp(const Domain<S1> &_value, const Domain<S2> &_min_value, const B &_max_value);

    /**
     * @brief Clamps a domain scalar @p _value between a raw numeric @p _min_value and a domain @p _max_value.
     *
     * @tparam S1 Type of @p _value; must satisfy @c MathTraits<S1>::supports_clip.
     * @tparam S2 Type of @p _max_value; must satisfy @c MathTraits<S2>::supports_clip.
     * @tparam B  Type of @p _min_value; must satisfy @ref is_numeric_v.
     */
    template <typename S1, typename S2, typename B, typename = std::enable_if_t<MathTraits<S1>::supports_clip && MathTraits<S2>::supports_clip && is_numeric_v<B>>>
    [[nodiscard]] Domain<S1> clamp(const Domain<S1> &_value, const B &_min_value, const Domain<S2> &_max_value);

    /**
     * @brief Clamps a domain scalar @p _value between raw numeric @p _min_value and @p _max_value.
     *
     * @tparam S  Type of @p _value; must satisfy @c MathTraits<S>::supports_clip.
     * @tparam B1 Type of @p _min_value; must satisfy @ref is_numeric_v.
     * @tparam B2 Type of @p _max_value; must satisfy @ref is_numeric_v.
     */
    template <typename S, typename B1, typename B2, typename = std::enable_if_t<MathTraits<S>::supports_clip && is_numeric_v<B1> && is_numeric_v<B2>>>
    [[nodiscard]] Domain<S> clamp(const Domain<S> &_value, const B1 &_min_value, const B2 &_max_value);

    // Optional: may add with _value(B) & _min/max_value(S)!? (total 3 more functions in that case)

    // =========================================================================
    // Trigonometric functions
    // =========================================================================

    /**
     * @brief Computes the sine of a domain scalar angle.
     * @tparam S Must satisfy @c MathTraits<S>::supports_trigonometric.
     */
    template <typename S, typename = std::enable_if_t<MathTraits<S>::supports_trigonometric>>
    [[nodiscard]] Domain<S> sin(const Domain<S> &_angle);

    /**
     * @brief Computes the cosine of a domain scalar angle.
     * @tparam S Must satisfy @c MathTraits<S>::supports_trigonometric.
     */
    template <typename S, typename = std::enable_if_t<MathTraits<S>::supports_trigonometric>>
    [[nodiscard]] Domain<S> cos(const Domain<S> &_angle);

    /**
     * @brief Computes the tangent of a domain scalar angle.
     * @tparam S Must satisfy @c MathTraits<S>::supports_trigonometric.
     */
    template <typename S, typename = std::enable_if_t<MathTraits<S>::supports_trigonometric>>
    [[nodiscard]] Domain<S> tan(const Domain<S> &_angle);

    /**
     * @brief Computes the cosecant of a domain scalar angle.
     * @tparam S Must satisfy @c MathTraits<S>::supports_trigonometric.
     */
    template <typename S, typename = std::enable_if_t<MathTraits<S>::supports_trigonometric>>
    [[nodiscard]] Domain<S> csc(const Domain<S> &_angle);

    /**
     * @brief Computes the secant of a domain scalar angle.
     * @tparam S Must satisfy @c MathTraits<S>::supports_trigonometric.
     */
    template <typename S, typename = std::enable_if_t<MathTraits<S>::supports_trigonometric>>
    [[nodiscard]] Domain<S> sec(const Domain<S> &_angle);

    /**
     * @brief Computes the cotangent of a domain scalar angle.
     * @tparam S Must satisfy @c MathTraits<S>::supports_trigonometric.
     */
    template <typename S, typename = std::enable_if_t<MathTraits<S>::supports_trigonometric>>
    [[nodiscard]] Domain<S> cot(const Domain<S> &_angle);

    // =========================================================================
    // Inverse trigonometric functions
    // =========================================================================

    /**
     * @brief Computes the arcsine of a domain scalar.
     * @tparam S Must satisfy @c MathTraits<S>::supports_trigonometric.
     */
    template <typename S, typename = std::enable_if_t<MathTraits<S>::supports_trigonometric>>
    [[nodiscard]] Domain<S> asin(const Domain<S> &_value);

    /**
     * @brief Computes the arccosine of a domain scalar.
     * @tparam S Must satisfy @c MathTraits<S>::supports_trigonometric.
     */
    template <typename S, typename = std::enable_if_t<MathTraits<S>::supports_trigonometric>>
    [[nodiscard]] Domain<S> acos(const Domain<S> &_value);

    /**
     * @brief Computes the arctangent of a domain scalar.
     * @tparam S Must satisfy @c MathTraits<S>::supports_trigonometric.
     */
    template <typename S, typename = std::enable_if_t<MathTraits<S>::supports_trigonometric>>
    [[nodiscard]] Domain<S> atan(const Domain<S> &_value);

    /**
     * @brief Computes the arccosecant of a domain scalar.
     * @tparam S Must satisfy @c MathTraits<S>::supports_trigonometric.
     */
    template <typename S, typename = std::enable_if_t<MathTraits<S>::supports_trigonometric>>
    [[nodiscard]] Domain<S> acsc(const Domain<S> &_value);

    /**
     * @brief Computes the arcsecant of a domain scalar.
     * @tparam S Must satisfy @c MathTraits<S>::supports_trigonometric.
     */
    template <typename S, typename = std::enable_if_t<MathTraits<S>::supports_trigonometric>>
    [[nodiscard]] Domain<S> asec(const Domain<S> &_value);

    /**
     * @brief Computes the arccotangent of a domain scalar.
     * @tparam S Must satisfy @c MathTraits<S>::supports_trigonometric.
     */
    template <typename S, typename = std::enable_if_t<MathTraits<S>::supports_trigonometric>>
    [[nodiscard]] Domain<S> acot(const Domain<S> &_value);

    // =========================================================================
    // Hyperbolic functions
    // =========================================================================

    /**
     * @brief Computes the hyperbolic sine of a domain scalar.
     * @tparam S Must satisfy @c MathTraits<S>::supports_hyperbolic.
     */
    template <typename S, typename = std::enable_if_t<MathTraits<S>::supports_hyperbolic>>
    [[nodiscard]] Domain<S> sinh(const Domain<S> &_value);

    /**
     * @brief Computes the hyperbolic cosine of a domain scalar.
     * @tparam S Must satisfy @c MathTraits<S>::supports_hyperbolic.
     */
    template <typename S, typename = std::enable_if_t<MathTraits<S>::supports_hyperbolic>>
    [[nodiscard]] Domain<S> cosh(const Domain<S> &_value);

    /**
     * @brief Computes the hyperbolic tangent of a domain scalar.
     * @tparam S Must satisfy @c MathTraits<S>::supports_hyperbolic.
     */
    template <typename S, typename = std::enable_if_t<MathTraits<S>::supports_hyperbolic>>
    [[nodiscard]] Domain<S> tanh(const Domain<S> &_value);

    /**
     * @brief Computes the hyperbolic cosecant of a domain scalar.
     * @tparam S Must satisfy @c MathTraits<S>::supports_hyperbolic.
     */
    template <typename S, typename = std::enable_if_t<MathTraits<S>::supports_hyperbolic>>
    [[nodiscard]] Domain<S> csch(const Domain<S> &_value);

    /**
     * @brief Computes the hyperbolic secant of a domain scalar.
     * @tparam S Must satisfy @c MathTraits<S>::supports_hyperbolic.
     */
    template <typename S, typename = std::enable_if_t<MathTraits<S>::supports_hyperbolic>>
    [[nodiscard]] Domain<S> sech(const Domain<S> &_value);

    /**
     * @brief Computes the hyperbolic cotangent of a domain scalar.
     * @tparam S Must satisfy @c MathTraits<S>::supports_hyperbolic.
     */
    template <typename S, typename = std::enable_if_t<MathTraits<S>::supports_hyperbolic>>
    [[nodiscard]] Domain<S> coth(const Domain<S> &_value);

    // =========================================================================
    // Inverse hyperbolic functions
    // =========================================================================

    /**
     * @brief Computes the inverse hyperbolic sine of a domain scalar.
     * @tparam S Must satisfy @c MathTraits<S>::supports_hyperbolic.
     */
    template <typename S, typename = std::enable_if_t<MathTraits<S>::supports_hyperbolic>>
    [[nodiscard]] Domain<S> asinh(const Domain<S> &_value);

    /**
     * @brief Computes the inverse hyperbolic cosine of a domain scalar.
     * @tparam S Must satisfy @c MathTraits<S>::supports_hyperbolic.
     */
    template <typename S, typename = std::enable_if_t<MathTraits<S>::supports_hyperbolic>>
    [[nodiscard]] Domain<S> acosh(const Domain<S> &_value);

    /**
     * @brief Computes the inverse hyperbolic tangent of a domain scalar.
     * @tparam S Must satisfy @c MathTraits<S>::supports_hyperbolic.
     */
    template <typename S, typename = std::enable_if_t<MathTraits<S>::supports_hyperbolic>>
    [[nodiscard]] Domain<S> atanh(const Domain<S> &_value);

    /**
     * @brief Computes the inverse hyperbolic cosecant of a domain scalar.
     * @tparam S Must satisfy @c MathTraits<S>::supports_hyperbolic.
     */
    template <typename S, typename = std::enable_if_t<MathTraits<S>::supports_hyperbolic>>
    [[nodiscard]] Domain<S> acsch(const Domain<S> &_value);

    /**
     * @brief Computes the inverse hyperbolic secant of a domain scalar.
     * @tparam S Must satisfy @c MathTraits<S>::supports_hyperbolic.
     */
    template <typename S, typename = std::enable_if_t<MathTraits<S>::supports_hyperbolic>>
    [[nodiscard]] Domain<S> asech(const Domain<S> &_value);

    /**
     * @brief Computes the inverse hyperbolic cotangent of a domain scalar.
     * @tparam S Must satisfy @c MathTraits<S>::supports_hyperbolic.
     */
    template <typename S, typename = std::enable_if_t<MathTraits<S>::supports_hyperbolic>>
    [[nodiscard]] Domain<S> acoth(const Domain<S> &_value);

    // =========================================================================
    // Complex-specific functions
    // =========================================================================

    /**
     * @brief Computes the norm (squared modulus) of a domain scalar.
     * @tparam S Scalar type; must satisfy @c MathTraits<S>::supports_modulus.
     * @return A real-valued domain holding the norm.
     */
    template <typename S, typename = std::enable_if_t<MathTraits<S>::supports_modulus>>
    [[nodiscard]] Domain<Real<typename ScalarTraits<S>::base_type>> norm(const Domain<S> &_x);

    /**
     * @brief Computes the argument (phase angle) of a domain scalar.
     * @tparam S Scalar type; must satisfy @c MathTraits<S>::supports_argument.
     * @return A real-valued domain holding the argument in radians.
     */
    template <typename S, typename = std::enable_if_t<MathTraits<S>::supports_argument>>
    [[nodiscard]] Domain<Real<typename ScalarTraits<S>::base_type>> argument(const Domain<S> &_value);

    /**
     * @brief Computes the complex conjugate of a domain scalar.
     * @tparam S Scalar type; must satisfy @c MathTraits<S>::supports_conjugate.
     */
    template <typename S, typename = std::enable_if_t<MathTraits<S>::supports_conjugate>>
    [[nodiscard]] Domain<S> conjugate(const Domain<S> &_value);

    // =========================================================================
    // Polar-form construction
    // =========================================================================

    /**
     * @brief Constructs a complex domain scalar from polar coordinates (real domain inputs).
     *
     * @tparam B1 Base type of @p _radius; must be floating-point.
     * @tparam B2 Base type of @p _angle; must be floating-point.
     * @param _radius The magnitude.
     * @param _angle  The phase angle, in radians.
     * @return A @c Domain<Complex<B>> equal to @c radius * (cos(angle) + i*sin(angle)).
     */
    template <typename B1, typename B2, typename = std::enable_if_t<std::is_floating_point_v<B1> && std::is_floating_point_v<B2>>>
    [[nodiscard]] Domain<Complex<common_float_t<B1, B2>>> polar(const Domain<Real<B1>> &_radius, const Domain<Real<B2>> &_angle);

    /**
     * @brief Constructs a complex domain scalar from polar coordinates (raw numeric inputs).
     *
     * @tparam B1 Type of @p _radius; must satisfy @ref is_numeric_v.
     * @tparam B2 Type of @p _angle; must satisfy @ref is_numeric_v.
     * @param _radius The magnitude.
     * @param _angle  The phase angle, in radians.
     */
    template <typename B1, typename B2, typename = std::enable_if_t<is_numeric_v<B1> && is_numeric_v<B2>>>
    [[nodiscard]] Domain<Complex<common_float_t<to_float_t<B1>, to_float_t<B2>>>> polar(const B1 &_radius, const B2 &_angle);

    // =========================================================================
    // Domain-promoted (Real -> Complex) functions
    // =========================================================================
    //
    // The following functions accept Real domain arguments but always return a
    // Complex domain, covering cases where a purely real result may not exist
    // (e.g. the square root or logarithm of a negative real, or the arcsine of
    // a real value outside [-1, 1]).

    /**
     * @brief Computes @p _base raised to @p _exponent, promoting the result to a complex domain.
     */
    template <typename B1, typename B2, typename = std::enable_if_t<std::is_floating_point_v<B1> && std::is_floating_point_v<B2>>>
    [[nodiscard]] Domain<Complex<common_float_t<B1, B2>>> pow_c(const Domain<Real<B1>> &_base, const Domain<Real<B2>> &_exponent);

    /**
     * @brief Computes a real domain @p _base raised to a raw numeric @p _exponent, promoting to a complex domain.
     */
    template <typename B1, typename B2, typename = std::enable_if_t<std::is_floating_point_v<B1> && is_numeric_v<B2>>>
    [[nodiscard]] Domain<Complex<common_float_t<B1, to_float_t<B2>>>> pow_c(const Domain<Real<B1>> &_base, const B2 &_exponent);

    /**
     * @brief Computes a raw numeric @p _base raised to a real domain @p _exponent, promoting to a complex domain.
     */
    template <typename B1, typename B2, typename = std::enable_if_t<std::is_floating_point_v<B1> && is_numeric_v<B2>>>
    [[nodiscard]] Domain<Complex<common_float_t<B1, to_float_t<B2>>>> pow_c(const B2 &_base, const Domain<Real<B1>> &_exponent);

    /**
     * @brief Computes the square root of a real domain, promoting to a complex domain (handles negative radicands).
     */
    template <typename B, typename = std::enable_if_t<std::is_floating_point_v<B>>>
    [[nodiscard]] Domain<Complex<B>> sqrt_c(const Domain<Real<B>> &_radicand);

    /**
     * @brief Computes the cube root of a real domain, promoting to a complex domain.
     */
    template <typename B, typename = std::enable_if_t<std::is_floating_point_v<B>>>
    [[nodiscard]] Domain<Complex<B>> cbrt_c(const Domain<Real<B>> &_radicand);

    /**
     * @brief Computes the logarithm of @p _argument with respect to @p _base, both real domains, promoting to a complex domain.
     */
    template <typename B1, typename B2, typename = std::enable_if_t<std::is_floating_point_v<B1> && std::is_floating_point_v<B2>>>
    [[nodiscard]] Domain<Complex<common_float_t<B1, B2>>> log_c(const Domain<Real<B1>> &_argument, const Domain<Real<B2>> &_base);

    /**
     * @brief Computes the logarithm of a real domain @p _argument with a raw numeric @p _base, promoting to a complex domain.
     */
    template <typename B1, typename B2, typename = std::enable_if_t<std::is_floating_point_v<B1> && is_numeric_v<B2>>>
    [[nodiscard]] Domain<Complex<common_float_t<B1, to_float_t<B2>>>> log_c(const Domain<Real<B1>> &_argument, const B2 &_base);

    /**
     * @brief Computes the logarithm of a raw numeric @p _argument with a real domain @p _base, promoting to a complex domain.
     */
    template <typename B1, typename B2, typename = std::enable_if_t<std::is_floating_point_v<B1> && is_numeric_v<B2>>>
    [[nodiscard]] Domain<Complex<common_float_t<B1, to_float_t<B2>>>> log_c(const B2 &_argument, const Domain<Real<B1>> &_base);

    /**
     * @brief Computes the natural logarithm of a real domain, promoting to a complex domain (handles negative arguments).
     */
    template <typename B, typename = std::enable_if_t<std::is_floating_point_v<B>>>
    [[nodiscard]] Domain<Complex<B>> ln_c(const Domain<Real<B>> &_argument);

    /**
     * @brief Computes the base-2 logarithm of a real domain, promoting to a complex domain.
     */
    template <typename B, typename = std::enable_if_t<std::is_floating_point_v<B>>>
    [[nodiscard]] Domain<Complex<B>> log2_c(const Domain<Real<B>> &_argument);

    /**
     * @brief Computes the base-10 logarithm of a real domain, promoting to a complex domain.
     */
    template <typename B, typename = std::enable_if_t<std::is_floating_point_v<B>>>
    [[nodiscard]] Domain<Complex<B>> log10_c(const Domain<Real<B>> &_argument);

    /**
     * @brief Computes the arcsine of a real domain, promoting to a complex domain (handles values outside [-1, 1]).
     */
    template <typename B, typename = std::enable_if_t<std::is_floating_point_v<B>>>
    [[nodiscard]] Domain<Complex<B>> asin_c(const Domain<Real<B>> &_value);

    /**
     * @brief Computes the arccosine of a real domain, promoting to a complex domain (handles values outside [-1, 1]).
     */
    template <typename B, typename = std::enable_if_t<std::is_floating_point_v<B>>>
    [[nodiscard]] Domain<Complex<B>> acos_c(const Domain<Real<B>> &_value);

    /**
     * @brief Computes the arccosecant of a real domain, promoting to a complex domain.
     */
    template <typename B, typename = std::enable_if_t<std::is_floating_point_v<B>>>
    [[nodiscard]] Domain<Complex<B>> acsc_c(const Domain<Real<B>> &_value);

    /**
     * @brief Computes the arcsecant of a real domain, promoting to a complex domain.
     */
    template <typename B, typename = std::enable_if_t<std::is_floating_point_v<B>>>
    [[nodiscard]] Domain<Complex<B>> asec_c(const Domain<Real<B>> &_value);

    /**
     * @brief Computes the inverse hyperbolic cosine of a real domain, promoting to a complex domain (handles values < 1).
     */
    template <typename B, typename = std::enable_if_t<std::is_floating_point_v<B>>>
    [[nodiscard]] Domain<Complex<B>> acosh_c(const Domain<Real<B>> &_value);

    /**
     * @brief Computes the inverse hyperbolic tangent of a real domain, promoting to a complex domain (handles values outside [-1, 1]).
     */
    template <typename B, typename = std::enable_if_t<std::is_floating_point_v<B>>>
    [[nodiscard]] Domain<Complex<B>> atanh_c(const Domain<Real<B>> &_value);

    /**
     * @brief Computes the inverse hyperbolic secant of a real domain, promoting to a complex domain.
     */
    template <typename B, typename = std::enable_if_t<std::is_floating_point_v<B>>>
    [[nodiscard]] Domain<Complex<B>> asech_c(const Domain<Real<B>> &_value);

    /**
     * @brief Computes the inverse hyperbolic cotangent of a real domain, promoting to a complex domain.
     */
    template <typename B, typename = std::enable_if_t<std::is_floating_point_v<B>>>
    [[nodiscard]] Domain<Complex<B>> acoth_c(const Domain<Real<B>> &_value);

    // Later addition: erf_inv(), lambert_w(), gamma(), factorial(), bessel_j(), bessel_y(), etc.
    // also their Complex Domain promotion functions.

} // namespace Scalar::Math

#include "ScalarMath.inl"
