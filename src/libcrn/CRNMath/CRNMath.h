/* Copyright 2006-2016 Yann LEYDIER, CoReNum, INSA-Lyon, Université Paris Descartes, ENS-Lyon
 *
 * This file is part of libcrn.
 *
 * libcrn is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libcrn is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with libcrn.  If not, see <http://www.gnu.org/licenses/>.
 *
 * file: CRNMath.h
 * \author Yann LEYDIER
 */

#ifndef CRNMath_HEADER
#define CRNMath_HEADER

#include <CRN.h>
#include <cmath>
#include <complex>
#include <algorithm>

#if defined(CRN_PF_ANDROID)
inline double log2(double n)
{
	return log(n) / log(2.0);
}
#endif

#ifndef M_PI
#   define M_PI 3.14159265358979323846
#endif

/*! \defgroup math Maths */
namespace crn
{
	/*! \addtogroup math */
	/*@{*/
	/*! \brief Returns the max of two values */
	template<typename T> inline const T& Max(const T &a, const T &b) { return std::max(a, b); }
	/*! \brief Returns the min of two values */
	template<typename T> inline const T& Min(const T &a, const T &b) { return std::min(a, b); }
	/*! \brief Returns the max of three values */
	template<typename T> inline const T& Max(const T &a, const T &b, const T&c) { return std::max(std::max(a, b), c); }
	/*! \brief Returns the min of three values */
	template<typename T> inline const T& Min(const T &a, const T &b, const T&c) { return std::min(std::min(a, b), c); }
	/*! \brief Returns the double of a value */
	template<typename T> constexpr typename TypeInfo<T>::SumType Twice(const T&v) noexcept(noexcept(v+v)) { return v + v; }
	/*! \brief Returns three times a value */
	template<typename T> constexpr typename TypeInfo<T>::SumType Thrice(const T&v) noexcept(noexcept(v+v)) { return v + v + v; }
	/*! \brief Returns the sign (-1 or 1) of a value */
	template<typename T> constexpr int SignOf(const T &x) noexcept(noexcept(x<0)) { return (x < 0) ? -1 : 1; }
	/*! \brief Returns the square of a value */
	template<typename T> constexpr SumType<T> Sqr(const T&v) noexcept(noexcept(v*v)) { return v * v; }
	/*! \brief Returns the absolute value of a value */
	template<typename T> constexpr T Abs(const T &v) noexcept(noexcept(v<0)&&noexcept(-v)) { return (v < 0) ? -v : v; }
	/*! \brief Returns the absolute value of a value */
	template<typename T> inline T Abs(const std::complex<T> &v) noexcept(noexcept(std::abs(std::complex<T>(0)))) { return std::abs(v); }
	/*! \brief Returns the value that has the maximal absolute value */
	template<typename T> inline T AbsMax(const T &a, const T &b) noexcept(noexcept(Abs(a))) { return Abs(a) >= Abs(b) ? a : b; }
	/*! \brief Returns the value that has the minimal absolute value */
	template<typename T> inline T AbsMin(const T &a, const T &b) noexcept(noexcept(Abs(a))) { return Abs(a) < Abs(b) ? a : b; }
	/*! \brief Returns the value that has the maximal absolute value if the values have the same sign */
	template<typename T> inline T AbsMaxSameSign(const T &a, const T &b) noexcept(noexcept(Abs(a))&&noexcept(SignOf(a))) { if (SignOf(a) == SignOf(b)) return Abs(a) >= Abs(b) ? a : b; else return T(0); }
	/*! \brief Returns the value that has the minimal absolute value if the values have the same sign */
	template<typename T> inline T AbsMinSameSign(const T &a, const T &b) noexcept(noexcept(Abs(a))&&noexcept(SignOf(a))) { if (SignOf(a) == SignOf(b)) return Abs(a) < Abs(b) ? a : b; else return T(0); }
	/*! \brief Bounds a value to an interval */
	template<typename T> inline const T& Cap(const T &v, const T &min, const T &max) { return Min(max, Max(min, v)); }

	/*! \brief Computes Gauss function at x for a given standard deviation (centered in 0) */
	inline double Gauss(double x, double sigma) { return exp((- x * x) / (2.0 * sigma * sigma)); }

	/*! \brief Computes Gauss function at x for a given standard deviation (centered in 0) – to use with matrices */
	inline double MeanGauss(double x, double sigma) { return (Gauss(x, sigma) + Gauss(x + 0.5, sigma) + Gauss(x - 0.5, sigma)) / 3.0; }

	/*! \brief Computes sqrt(a²+b²) without destructive underflow or overflow */
	inline double Pythagoras(double a, double b) noexcept { double aa = Abs(a), ab = Abs(b); return (aa  > ab) ? aa * sqrt(1.0 + Sqr(ab / aa)) : ((ab == 0.0) ? 0.0 : ab * sqrt(1.0 + Sqr(aa / ab))); }
	/*! \brief Computes sqrt(a²+b²) without destructive underflow or overflow */
	inline double Pythagoras(int a, int b) noexcept { double aa = Abs(a), ab = Abs(b); return (aa  > ab) ? aa * sqrt(1.0 + Sqr(ab / aa)) : ((ab == 0.0) ? 0.0 : ab * sqrt(1.0 + Sqr(aa / ab))); }
	
    /*! \brief Computes sqrt(a²+b²+c²) */
    inline double Pythagoras(double a, double b, double c) noexcept { return sqrt(Sqr(a) + Sqr(b) + Sqr(c)); }
    
    /*! \brief Computes euclidean norm */
    template <class ITER> inline double Pythagoras(ITER it_begin, ITER it_end)
    {
        auto cumul_squares = 0.0;
        
        for (ITER it = it_begin; it != it_end; ++it)
            cumul_squares += Sqr(*it);
        
        return sqrt(cumul_squares);
    }
    
    /*! \brief Scale a collection of numbers */
    template <class ITER> inline void Scale(ITER it_begin, ITER it_end, const double s)
    {
        for (ITER it = it_begin; it != it_end; ++it)
            (*it) *= s;
    }

	/*@}*/
}

#include <CRNMath/CRNTrigonometry.h>

namespace crn
{
	/*! \addtogroup math */
	/*@{*/
	/*! \brief An enumeration of directions
	 *
	 * An enumeration of directions
	 */
	enum class Direction { INVALID = 0, LEFT = 1, RIGHT = 2, TOP = 4, BOTTOM = 8 };
	template<typename Unit> Direction ToDirection(const Angle<Unit> &ang) noexcept
	{
		Angle<ByteAngle> a(ang);
		if (a == Angle<ByteAngle>::LEFT())
		{
			return Direction::LEFT;
		}
		else if (a == Angle<ByteAngle>::RIGHT())
		{
			return Direction::RIGHT;
		}
		else if (a == Angle<ByteAngle>::TOP())
		{
			return Direction::TOP;
		}
		else if (a == Angle<ByteAngle>::BOTTOM())
		{
			return Direction::BOTTOM;
		}
		else
		{
			return Direction::INVALID;
		}
	}

	/*! \brief An enumeration of orientations
	 *
	 * An enumeration of orientations
	 */
	enum class Orientation { INVALID = 0, HORIZONTAL, VERTICAL };
	Orientation ToOrientation(Direction d) noexcept;

	/*! \brief An enumeration of distances
	 *
	 * An enumeration of distances
	 */
	enum class DistanceType { INVALID = 0, D4, D8, EUCLIDEAN };

	/*! \brief Distance between two numbers */
	template<
		typename T,
		typename std::enable_if<std::is_arithmetic<T>::value, int>::type = 0
		>
	double Distance(T o1, T o2)
	{
	 return double(Abs(o1 - o2));
	}
	
	/*@}*/

}

CRN_DECLARE_ENUM_OPERATORS(crn::Direction)
CRN_DECLARE_ENUM_OPERATORS(crn::Orientation)

#endif


