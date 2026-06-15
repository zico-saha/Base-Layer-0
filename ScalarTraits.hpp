/**
 * @file ScalarTraits.hpp
 * @brief Type traits and promotion utilities for real and complex scalar types.
 *
 * This header defines the core scalar type system used throughout the library.
 * It provides:
 * - @ref Scalar::Real and @ref Scalar::Complex class templates (forward declared)
 * - @ref Scalar::ScalarTraits specialisations describing field properties
 * - @ref Scalar::MathTraits specialisations describing supported mathematical operations
 * - Type promotion rules via @ref Scalar::Promote
 * - Utility type aliases and numeric predicates
 *
 * @note All components reside in the @ref Scalar namespace.
 */

#pragma once

#include <cmath>
#include <type_traits>
#include <utility>

/**
 * @namespace Scalar
 * @brief Namespace encapsulating the scalar type system, traits, and promotion utilities.
 */
namespace Scalar
{
    // =========================================================================
    // Forward declarations
    // =========================================================================

    /**
     * @brief Forward declaration of the real scalar wrapper.
     * @tparam S The underlying numeric base type (e.g. @c float, @c double).
     */
    template <typename S>
    class Real;

    /**
     * @brief Forward declaration of the complex scalar wrapper.
     * @tparam S The underlying numeric base type (e.g. @c float, @c double).
     */
    template <typename S>
    class Complex;

    // =========================================================================
    // Field tags
    // =========================================================================

    /**
     * @brief Tag type identifying a real number field.
     *
     * Used as the @c field_tag member in @ref ScalarTraits specialisations for
     * @ref Real types. Enables tag-dispatch on the algebraic field at compile time.
     */
    struct RealFieldTag
    {
    };

    /**
     * @brief Tag type identifying a complex number field.
     *
     * Used as the @c field_tag member in @ref ScalarTraits specialisations for
     * @ref Complex types. Enables tag-dispatch on the algebraic field at compile time.
     */
    struct ComplexFieldTag
    {
    };

    // =========================================================================
    // ScalarTraits
    // =========================================================================

    /**
     * @brief Primary (unspecialised) template for scalar type traits.
     *
     * Instantiating this primary template for an unsupported type @p B triggers
     * a @c static_assert, enforcing that only explicitly specialised scalar
     * types may be used with the traits system.
     *
     * @tparam B The scalar type to query. Must be @ref Real<T> or @ref Complex<T>.
     */
    template <typename B>
    struct ScalarTraits
    {
        static_assert(sizeof(B) == 0, "ScalarTraits is not specialised for this type.");
    };

    /**
     * @brief Scalar traits specialisation for @ref Real types.
     *
     * Provides compile-time constants and type aliases that describe the
     * algebraic and analytic properties of a real scalar.
     *
     * @tparam B The underlying numeric base type of the real scalar.
     */
    template <typename B>
    struct ScalarTraits<Real<B>>
    {
        /// The underlying numeric base type (e.g. @c float, @c double).
        using base_type = B;

        /// Field tag identifying this as a real number field (@ref RealFieldTag).
        using field_tag = RealFieldTag;

        /// @c true — @ref Real is a real-valued type.
        static constexpr bool is_real = true;

        /// @c true — @ref Real supports a total order (@c < , @c > , etc.).
        static constexpr bool is_ordered = true;

        /// @c true — @ref Real has a modulus (absolute value).
        static constexpr bool has_modulus = true;

        /// @c false — @ref Real has no complex argument (phase angle).
        static constexpr bool has_argument = false;

        /// @c false — @ref Real has no complex conjugate operation.
        static constexpr bool has_conjugate = false;

        /// @brief Returns the additive identity (0) for this type.
        static constexpr Real<B> zero() noexcept;

        /// @brief Returns the multiplicative identity (1) for this type.
        static constexpr Real<B> one() noexcept;
    };

    /**
     * @brief Scalar traits specialisation for @ref Complex types.
     *
     * Provides compile-time constants and type aliases that describe the
     * algebraic and analytic properties of a complex scalar.
     *
     * @tparam B The underlying numeric base type of the complex scalar.
     */
    template <typename B>
    struct ScalarTraits<Complex<B>>
    {
        /// The underlying numeric base type (e.g. @c float, @c double).
        using base_type = B;

        /// Field tag identifying this as a complex number field (@ref ComplexFieldTag).
        using field_tag = ComplexFieldTag;

        /// @c false — @ref Complex is not a real-valued type.
        static constexpr bool is_real = false;

        /// @c false — @ref Complex does not support a total order.
        static constexpr bool is_ordered = false;

        /// @c true — @ref Complex has a modulus (magnitude).
        static constexpr bool has_modulus = true;

        /// @c true — @ref Complex has a complex argument (phase angle).
        static constexpr bool has_argument = true;

        /// @c true — @ref Complex supports complex conjugation.
        static constexpr bool has_conjugate = true;

        /// @brief Returns the additive identity (0 + 0i) for this type.
        static constexpr Complex<B> zero() noexcept;

        /// @brief Returns the multiplicative identity (1 + 0i) for this type.
        static constexpr Complex<B> one() noexcept;

        /// @brief Returns the imaginary unit (0 + 1i).
        static constexpr Complex<B> i() noexcept;
    };

    // =========================================================================
    // Utility type aliases
    // =========================================================================

    /**
     * @brief Maps an integral base type to @c float; leaves floating-point types unchanged.
     *
     * This alias is used to ensure that arithmetic on scalar wrappers always
     * operates in floating-point.
     *
     * @tparam B The candidate base type.
     *
     * @par Example
     * @code
     * static_assert(std::is_same_v<to_float_t<int>,    float>);   // integral → float
     * static_assert(std::is_same_v<to_float_t<double>, double>);  // fp → unchanged
     * @endcode
     */
    template <typename B>
    using to_float_t = std::conditional_t<std::is_integral_v<B>, float, B>;

    /**
     * @brief The common floating-point type between two base types.
     *
     * Resolves to @c std::common_type_t<B1, B2>, which follows the usual
     * arithmetic conversion rules (e.g. @c float × @c double → @c double).
     *
     * @tparam B1 First base type.
     * @tparam B2 Second base type.
     */
    template <typename B1, typename B2>
    using common_float_t = std::common_type_t<B1, B2>;

    // =========================================================================
    // Promote — type promotion for mixed scalar expressions
    // =========================================================================

    /**
     * @brief Primary (unspecialised) template for scalar type promotion.
     *
     * Determines the result type when combining two scalar types in an
     * arithmetic expression. Specialisations implement the following promotion
     * hierarchy:
     *   - Real × Real   → Real   (with common base type)
     *   - Complex × Complex → Complex (with common base type)
     *   - Real × Complex (either order) → Complex (with common base type)
     *
     * Use the convenience alias @ref promote_t to access the result type.
     *
     * @tparam S1 First scalar type.
     * @tparam S2 Second scalar type.
     */
    template <typename S1, typename S2>
    struct Promote;

    /**
     * @brief Promotion of two @ref Real types.
     *
     * @tparam B1 Base type of the first operand.
     * @tparam B2 Base type of the second operand.
     */
    template <typename B1, typename B2>
    struct Promote<Real<B1>, Real<B2>>
    {
        /// Result type: Real with the common base type of @p B1 and @p B2.
        using type = Real<common_float_t<B1, B2>>;
    };

    /**
     * @brief Promotion of two @ref Complex types.
     *
     * @tparam B1 Base type of the first operand.
     * @tparam B2 Base type of the second operand.
     */
    template <typename B1, typename B2>
    struct Promote<Complex<B1>, Complex<B2>>
    {
        /// Result type: Complex with the common base type of @p B1 and @p B2.
        using type = Complex<common_float_t<B1, B2>>;
    };

    /**
     * @brief Promotion of a @ref Real and a @ref Complex type.
     *
     * A real value widens to complex when combined with a complex operand.
     *
     * @tparam B1 Base type of the Real operand.
     * @tparam B2 Base type of the Complex operand.
     */
    template <typename B1, typename B2>
    struct Promote<Real<B1>, Complex<B2>>
    {
        /// Result type: Complex with the common base type of @p B1 and @p B2.
        using type = Complex<common_float_t<B1, B2>>;
    };

    /**
     * @brief Promotion of a @ref Complex and a @ref Real type (symmetric case).
     *
     * Delegates to @ref Promote<Real, Complex> for consistency.
     *
     * @tparam B1 Base type of the Complex operand.
     * @tparam B2 Base type of the Real operand.
     */
    template <typename B1, typename B2>
    struct Promote<Complex<B1>, Real<B2>> : Promote<Real<B2>, Complex<B1>>
    {
    };

    /**
     * @brief Convenience alias for the promoted scalar type of @p S1 and @p S2.
     *
     * @tparam S1 First scalar type.
     * @tparam S2 Second scalar type.
     *
     * @par Example
     * @code
     * using T = promote_t<Real<float>, Complex<double>>; // Complex<double>
     * @endcode
     */
    template <typename S1, typename S2>
    using promote_t = typename Promote<S1, S2>::type;

    // =========================================================================
    // IsNumeric — numeric type predicate
    // =========================================================================

    /**
     * @brief Predicate that is @c true for numeric arithmetic types, excluding @c bool.
     *
     * Inherits from @c std::bool_constant and evaluates to @c true when @p B
     * satisfies @c std::is_arithmetic and is not @c bool.
     *
     * @tparam B The type to test.
     */
    template <typename B>
    struct IsNumeric : std::bool_constant<std::is_arithmetic_v<B> && !std::is_same_v<B, bool>>
    {
    };

    /**
     * @brief Convenience variable template for @ref IsNumeric.
     *
     * @tparam B The type to test.
     *
     * @par Example
     * @code
     * static_assert( is_numeric_v<double>);  // true
     * static_assert(!is_numeric_v<bool>);    // false
     * static_assert(!is_numeric_v<void>);    // false
     * @endcode
     */
    template <typename B>
    inline constexpr bool is_numeric_v = IsNumeric<B>::value;

    // =========================================================================
    // IsAllowedConversion — conversion permission predicate
    // =========================================================================

    /**
     * @brief Predicate indicating whether an implicit conversion from @p From to @p To is permitted.
     *
     * The primary template allows all conversions. The specialisation below
     * prohibits implicit narrowing from @ref Complex to @ref Real, since such a
     * conversion is lossy (the imaginary component is discarded).
     *
     * @tparam From Source scalar type.
     * @tparam To   Destination scalar type.
     */
    template <typename From, typename To>
    struct IsAllowedConversion : std::true_type
    {
    };

    /**
     * @brief Specialisation that forbids implicit conversion from @ref Complex to @ref Real.
     *
     * Converting a complex value to a real one silently drops the imaginary part,
     * which is almost always a programming error. This specialisation makes the
     * intent explicit and allows consuming code to produce a diagnostic.
     *
     * @tparam B1 Base type of the Complex source.
     * @tparam B2 Base type of the Real destination.
     */
    template <typename B1, typename B2>
    struct IsAllowedConversion<Complex<B1>, Real<B2>> : std::false_type
    {
    };

    // =========================================================================
    // MathTraits — supported mathematical operations
    // =========================================================================

    /**
     * @brief Primary (unspecialised) template for mathematical operation support traits.
     *
     * All capability flags default to @c false. Specialisations for @ref Real and
     * @ref Complex selectively enable the operations each type supports.
     *
     * @tparam B The scalar type to query.
     */
    template <typename B>
    struct MathTraits
    {
        static constexpr bool supports_polar         = false; ///< Polar (magnitude/angle) construction.
        static constexpr bool supports_sqrt          = false; ///< Square root.
        static constexpr bool supports_pow           = false; ///< Power function.
        static constexpr bool supports_exp           = false; ///< Exponential function.
        static constexpr bool supports_log           = false; ///< Natural logarithm.
        static constexpr bool supports_modulus       = false; ///< Modulus (absolute value / magnitude).
        static constexpr bool supports_argument      = false; ///< Complex argument (phase angle).
        static constexpr bool supports_conjugate     = false; ///< Complex conjugate.
        static constexpr bool supports_floor         = false; ///< Floor rounding.
        static constexpr bool supports_ceil          = false; ///< Ceiling rounding.
        static constexpr bool supports_round         = false; ///< Nearest-integer rounding.
        static constexpr bool supports_clip          = false; ///< Clamping to a range.
        static constexpr bool supports_modulo_op     = false; ///< Modulo (remainder) operation.
        static constexpr bool supports_trigonometric = false; ///< Trigonometric functions (sin, cos, tan, …).
        static constexpr bool supports_hyperbolic    = false; ///< Hyperbolic functions (sinh, cosh, tanh, …).
    };

    /**
     * @brief MathTraits specialisation for @ref Real types.
     *
     * Real scalars support all standard mathematical operations available to
     * floating-point values. Polar construction is excluded because it is
     * specific to complex arithmetic.
     *
     * @tparam B The underlying numeric base type.
     */
    template <typename B>
    struct MathTraits<Real<B>>
    {
        static constexpr bool supports_polar         = false; ///< Not applicable to real scalars.
        static constexpr bool supports_sqrt          = true;  ///< Square root.
        static constexpr bool supports_pow           = true;  ///< Power function.
        static constexpr bool supports_exp           = true;  ///< Exponential function.
        static constexpr bool supports_log           = true;  ///< Natural logarithm.
        static constexpr bool supports_modulus       = true;  ///< Absolute value.
        static constexpr bool supports_argument      = true;  ///< Sign (treated as 0 or π).
        static constexpr bool supports_conjugate     = true;  ///< Identity for real values.
        static constexpr bool supports_floor         = true;  ///< Floor rounding.
        static constexpr bool supports_ceil          = true;  ///< Ceiling rounding.
        static constexpr bool supports_round         = true;  ///< Nearest-integer rounding.
        static constexpr bool supports_clip          = true;  ///< Clamping to a range.
        static constexpr bool supports_modulo_op     = true;  ///< Modulo (remainder) operation.
        static constexpr bool supports_trigonometric = true;  ///< Trigonometric functions.
        static constexpr bool supports_hyperbolic    = true;  ///< Hyperbolic functions.
    };

    /**
     * @brief MathTraits specialisation for @ref Complex types.
     *
     * Complex scalars support polar construction, most transcendental functions,
     * modulus, argument, conjugate, and component-wise rounding. Operations that
     * require a total order (floor, ceil, clip) or integer semantics (modulo) are
     * not defined for complex numbers and are therefore disabled.
     *
     * @tparam B The underlying numeric base type.
     */
    template <typename B>
    struct MathTraits<Complex<B>>
    {
        static constexpr bool supports_polar         = true;  ///< Polar (magnitude/angle) construction.
        static constexpr bool supports_sqrt          = true;  ///< Square root.
        static constexpr bool supports_pow           = true;  ///< Power function.
        static constexpr bool supports_exp           = true;  ///< Exponential function.
        static constexpr bool supports_log           = true;  ///< Natural logarithm (principal value).
        static constexpr bool supports_modulus       = true;  ///< Magnitude.
        static constexpr bool supports_argument      = true;  ///< Phase angle.
        static constexpr bool supports_conjugate     = true;  ///< Complex conjugate.
        static constexpr bool supports_floor         = false; ///< Undefined for complex numbers.
        static constexpr bool supports_ceil          = false; ///< Undefined for complex numbers.
        static constexpr bool supports_round         = true;  ///< Component-wise rounding.
        static constexpr bool supports_clip          = false; ///< Undefined (no total order).
        static constexpr bool supports_modulo_op     = false; ///< Undefined for complex numbers.
        static constexpr bool supports_trigonometric = true;  ///< Trigonometric functions.
        static constexpr bool supports_hyperbolic    = true;  ///< Hyperbolic functions.
    };

} // namespace Scalar
