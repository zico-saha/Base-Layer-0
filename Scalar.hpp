/**
 * @file Scalar.hpp
 * @brief Umbrella header for the Scalar library.
 *
 * Including this single header pulls in the complete public API:
 * - @ref Scalar::Real, @ref Scalar::Complex — zero-overhead scalar types
 *   (see ScalarTypes.hpp)
 * - @ref Scalar::ScalarTraits, @ref Scalar::MathTraits — compile-time type
 *   property traits (see ScalarTraits.hpp)
 * - @ref Scalar::Domain — named-domain wrapper providing the day-to-day
 *   arithmetic and comparison interface (see Domain.hpp)
 * - @ref Scalar::Math — free-function mathematical operations (pow, sqrt,
 *   exp, log, trigonometric, hyperbolic, etc.) over @ref Scalar::Domain
 *   values (see ScalarMath.hpp)
 *
 * @par Example
 * @code
 * #include "Scalar.hpp"
 * using namespace Scalar;
 *
 * RealScalar<double>    frequency(440.0);
 * ComplexScalar<double> impedance(50.0, -10.0);
 *
 * auto magnitude = Math::abs(impedance);
 * auto doubled   = frequency * 2.0;
 * @endcode
 *
 * @note Prefer including this umbrella header over individual component
 *       headers (ScalarTraits.hpp, ScalarBase.hpp, ScalarTypes.hpp,
 *       Domain.hpp, ScalarMath.hpp) unless you specifically need to limit
 *       your translation unit's dependencies.
 */

#pragma once

#include "Domain.hpp"
#include "ScalarMath.hpp"
