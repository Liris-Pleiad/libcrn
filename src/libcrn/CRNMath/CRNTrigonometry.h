/* Copyright 2006-2015 Yann LEYDIER, CoReNum, Université Paris Descartes
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
 * file: CRNTrigonometry.h
 * \author Yann LEYDIER
 */

#ifndef CRNTrigonometry_HEADER
#define CRNTrigonometry_HEADER

#include <CRN.h>
#include <CRNException.h>
#include <cmath>
#include <vector>

/*! \defgroup math Maths */
namespace crn
{
	/*! \addtogroup math */
	/*@{*/
	/*! \brief Radian angle unit */
	struct Radian
	{
		using type = double;
		static const type MAXVAL;
		static const TypeInfo<type>::SumType VAL2PI;
	};
	/*! \brief Degree angle unit */
	struct Degree
	{
		using type = double;
		static const type MAXVAL;
		static const TypeInfo<type>::SumType VAL2PI;
	};
	/*! \brief BYTE angle unit */
	struct ByteAngle
	{
		using type = uint8_t;
		static const type MAXVAL;
		static const TypeInfo<type>::SumType VAL2PI;
	};

	/*! \brief Cosine of an angle 
	 * \param[in]	angle	the angle in the unit given in template
	 * \return	the cosine of the angle
	 */
	template<typename Unit> inline double Cosine(typename Unit::type angle) noexcept
	{
		return cos(angle * Radian::MAXVAL / Unit::MAXVAL);
	}

	/*! \brief Sine of an angle 
	 * \param[in]	angle	the angle in the unit given in template
	 * \return	the sine of the angle
	 */
	template<typename Unit> inline double Sine(typename Unit::type angle) noexcept
	{
		return sin(angle * Radian::MAXVAL / Unit::MAXVAL);
	}

	/*! \brief Tangent of an angle 
	 * \param[in]	angle	the angle in the unit given in template
	 * \return	the tangent of the angle
	 */
	template<typename Unit> inline double Tangent(typename Unit::type angle) noexcept
	{
		return tan(angle * Radian::MAXVAL / Unit::MAXVAL);
	}

	/*! \brief Cosine of a byte angle using a lookup table
	 * \param[in]	angle	the angle in [0..255]
	 * \return	the cosine of the angle
	 */
	template<> inline double Cosine<ByteAngle>(uint8_t angle) noexcept
	{
		static double costab[256];
		static bool init = false;
		if (!init)
		{
			init = true;
			for (int tmp = 0; tmp < 256; ++tmp)
				costab[tmp] = cos(tmp * Radian::MAXVAL / ByteAngle::MAXVAL);
		}
		return costab[angle];
	}

	/*! \brief Sine of a byte angle using a lookup table
	 * \param[in]	angle	the angle in [0..255]
	 * \return	the sine of the angle
	 */
	template<> inline double Sine<ByteAngle>(uint8_t angle) noexcept
	{
		static double sintab[256];
		static bool init = false;
		if (!init)
		{
			init = true;
			for (int tmp = 0; tmp < 256; ++tmp)
				sintab[tmp] = sin(tmp * Radian::MAXVAL / ByteAngle::MAXVAL);
		}
		return sintab[angle];
	}

	/*! \brief Tangent of a byte angle using a lookup table
	 * \param[in]	angle	the angle in [0..255]
	 * \return	the tangent of the angle
	 */
	template<> inline double Tangent<ByteAngle>(uint8_t angle) noexcept
	{
		static double tantab[256];
		static bool init = false;
		if (!init)
		{
			init = true;
			for (int tmp = 0; tmp < 256; ++tmp)
				tantab[tmp] = tan(tmp * Radian::MAXVAL / ByteAngle::MAXVAL);
		}
		return tantab[angle];
	}

	/*! \brief A convenience class for angles units
	 *
	 * A convenience class for angles units
	 */
	template<typename Unit> struct Angle
	{
		using unit = Unit;
		/*! \brief Null angle */
		constexpr Angle() noexcept(std::is_nothrow_constructible<typename Unit::type>::value) 
			: value(0) { }
		/*! \brief Constructor from a value */
		inline Angle(typename Unit::type val) noexcept(std::is_nothrow_constructible<typename Unit::type>::value)
			: value(val)
		{
			typename TypeInfo<typename Unit::type>::SumType tmp(value);
			while (tmp >= Unit::VAL2PI)
				tmp -= typename TypeInfo<typename Unit::type>::SumType(Unit::VAL2PI);
			while (tmp < 0)
				tmp += Unit::VAL2PI;
			value = typename Unit::type(tmp);
		}
		/*! \brief Constructor from any other angle unit */
		template<typename U> inline Angle(const Angle<U> &other) noexcept(
				std::is_nothrow_constructible<typename U::type>::value && 
				std::is_nothrow_constructible<typename TypeInfo<typename Unit::type>::SumType>::value) 
			: value(other.template Get<Unit>())
		{
			typename TypeInfo<typename Unit::type>::SumType tmp(value);
			while (tmp >= Unit::VAL2PI)
				tmp -= Unit::VAL2PI;
			while (tmp < 0)
				tmp += Unit::VAL2PI;
			value = typename Unit::type(tmp);
		}
		Angle(const Angle&) = default;
		Angle(Angle&&) = default;
		Angle& operator=(const Angle&) = default;
		Angle& operator=(Angle&&) = default;

		/*! \brief Conversion to any other angle unit */
		template<typename U> inline typename U::type Get() const noexcept(
				std::is_nothrow_constructible<typename U::type>::value && 
				std::is_nothrow_constructible<typename TypeInfo<typename Unit::type>::SumType>::value)
		{
			return typename U::type(((typename TypeInfo<typename Unit::type>::SumType)value * U::MAXVAL) / Unit::MAXVAL);
		}

		/*! \brief Negative of the angle */
		inline Angle operator-() const noexcept(std::is_nothrow_copy_assignable<typename Unit::type>::value) 
		{ Angle a(Unit::VAL2PI); return a -= *this; }

		/*! \brief Adds an angle */
		inline const Angle& operator+=(typename Unit::type angle) noexcept(std::is_nothrow_copy_assignable<typename Unit::type>::value)
		{
			typename TypeInfo<typename Unit::type>::SumType tmp(value);
			tmp += angle;
			while (tmp >= Unit::VAL2PI)
				tmp -= Unit::VAL2PI;
			value = typename Unit::type(tmp);
			return *this;
		}
		/*! \brief Adds an angle */
		inline const Angle& operator+=(const Angle &other) noexcept(std::is_nothrow_copy_assignable<typename Unit::type>::value) { return this->operator+=(other.value); }
		/*! \brief Adds an angle */
		inline Angle operator+(const Angle &other) const  noexcept(std::is_nothrow_copy_assignable<typename Unit::type>::value){ Angle a(*this); return  a+= other; }

		/*! \brief Subtracts an angle */
		inline const Angle& operator-=(typename Unit::type angle) noexcept(std::is_nothrow_copy_assignable<typename Unit::type>::value)
		{
			typename TypeInfo<typename Unit::type>::DiffType tmp(value);
			tmp -= angle;
			while (tmp < 0)
				tmp += Unit::VAL2PI;
			value = typename Unit::type(tmp);
			return *this;
		}
		/*! \brief Subtracts an angle */
		inline const Angle& operator-=(const Angle &other) noexcept(std::is_nothrow_copy_assignable<typename Unit::type>::value) 
		{ return this->operator-=(other.value); }
		/*! \brief Subtracts an angle */
		inline Angle operator-(const Angle &other) const noexcept(std::is_nothrow_copy_assignable<typename Unit::type>::value)
		{ Angle a(*this); return a -= other; }

		/*! \brief Multiplies by a scalar */
		inline Angle& operator*=(double f) noexcept(std::is_nothrow_copy_assignable<typename Unit::type>::value)
		{
			auto tmp = double(value) * f; 
			while (tmp >= Unit::VAL2PI)
				tmp -= Unit::VAL2PI;
			while (tmp < 0)
				tmp += Unit::VAL2PI;
			value = typename Unit::type(tmp);
			return *this;
		}
		/*! \brief Multiplies by a scalar */
		inline Angle& operator*(double f) noexcept(std::is_nothrow_copy_assignable<typename Unit::type>::value) { auto a(*this); return a *= f; }

		/*! \brief Comparison operator */
		inline bool operator==(const Angle &other) const noexcept { return value == other.value; }

		/*! \brief Computes cosine */
		inline double Cos() const noexcept { return crn::Cosine<Unit>(value); }
		/*! \brief Computes sine */
		inline double Sin() const noexcept { return crn::Sine<Unit>(value); }
		/*! \brief Computes tangent */
		inline double Tan() const noexcept { return crn::Tangent<Unit>(value); }

		/*! \brief Computes arc cosine */
		static inline Angle Acos(double cosine) noexcept(std::is_nothrow_constructible<typename Unit::type>::value) { return Angle<Radian>(acos(cosine)); }
		/*! \brief Computes arc sine */
		static inline Angle Asin(double sine) noexcept(std::is_nothrow_constructible<typename Unit::type>::value) { return Angle<Radian>(asin(sine)); }
		/*! \brief Computes arc tangent */
		static inline Angle Atan(double tangent) noexcept(std::is_nothrow_constructible<typename Unit::type>::value) { return Angle<Radian>(atan(tangent)); }
		/*! \brief Computes arc tangent from coordinates */
		static inline Angle Atan(double y, double x) noexcept(std::is_nothrow_constructible<typename Unit::type>::value) { return Angle<Radian>(atan2(y, x)); }

		static const Angle<Unit> LEFT; /*!< Angle facing left (automatically computed) */
		static const Angle<Unit> RIGHT; /*!< Angle facing right (automatically computed) */
		static const Angle<Unit> TOP; /*!< Angle facing top (automatically computed) */
		static const Angle<Unit> BOTTOM; /*!< Angle facing bottom (automatically computed) */

		typename Unit::type value; /*!< The value of the angle */
	};

	template<typename Unit> const Angle<Unit> Angle<Unit>::LEFT(typename Unit::type(0));
	template<typename Unit> const Angle<Unit> Angle<Unit>::RIGHT(typename Unit::type(Unit::MAXVAL / 2));
	template<typename Unit> const Angle<Unit> Angle<Unit>::TOP(typename Unit::type(Unit::MAXVAL / 4));
	template<typename Unit> const Angle<Unit> Angle<Unit>::BOTTOM(typename Unit::type((3 * (typename TypeInfo<typename Unit::type>::SumType)Unit::MAXVAL) / 4));

	template<class ANGLE> using Unit = typename ANGLE::unit;

	/*! \brief Adds two angles */
	template<typename Unit> inline Angle<Unit> operator+(const Angle<Unit> &a1, const Angle<Unit> &a2) 
		noexcept(
				std::is_nothrow_constructible<typename Unit::type>::value && 
				std::is_nothrow_copy_assignable<typename Unit::type>::value)
	{
		Angle<Unit> tmp(a1);
		tmp += a2;
		return tmp;
	}

	/*! \brief Subtracts two angles */
	template<typename Unit> inline Angle<Unit> operator-(const Angle<Unit> &a1, const Angle<Unit> &a2) 
		noexcept(
				std::is_nothrow_constructible<typename Unit::type>::value && 
				std::is_nothrow_copy_assignable<typename Unit::type>::value)
	{
		Angle<Unit> tmp(a1);
		tmp -= a2;
		return tmp;
	}

	/*! \brief Multiplies an scalar with an angle */
	template<typename Unit> inline Angle<Unit> operator*(double f, const Angle<Unit> &a) 
		noexcept(
				std::is_nothrow_constructible<typename Unit::type>::value && 
				std::is_nothrow_copy_assignable<typename Unit::type>::value)
	{
		Angle<Unit> tmp(a);
		return tmp *= f;
	}

	namespace literals
	{
		inline Angle<Degree> operator"" _deg(long double val)
		{
			return Angle<Degree>{double(val)};
		}
		inline Angle<Radian> operator"" _rad(long double val)
		{
			return Angle<Radian>{double(val)};
		}
		inline Angle<ByteAngle> operator"" _ba(unsigned long long int val)
		{
			return Angle<ByteAngle>{uint8_t(val % 256)};
		}
	}

	/*! \return	the cosine of the angle */
	template<class A> inline double Cos(const A &a) noexcept { return a.Cos(); }
	/*! \return	the cosine of the angle */
	inline double Cos(double a) noexcept { return cos(a); }
	/*! \return	the cosine of the angle */
	inline double Cos(uint8_t a) noexcept { return Cosine<ByteAngle>(a); }
	/*! \return	the sine of the angle */
	template<class A> inline double Sin(const A &a) noexcept { return a.Sin(); }
	/*! \return	the sine of the angle */
	inline double Sin(double a) noexcept { return sin(a); }
	/*! \return	the sine of the angle */
	inline double Sin(uint8_t a) noexcept { return Sine<ByteAngle>(a); }
	/*! \return	the tangent of the angle */
	template<class A> inline double Tan(const A &a) noexcept { return a.Tan(); }
	/*! \return	the tangent of the angle */
	inline double Tan(double a) noexcept { return Tan(a); }
	/*! \return	the tangent of the angle */
	inline double Tan(uint8_t a) noexcept { return Tangent<ByteAngle>(a); }
	/*! \return	the arctangent of a pair or sine/cosine */
	template<class A> inline A Atan(double s, double c) noexcept { return A::Atan(s, c); }
	/*! \return	the arctangent of a pair or sine/cosine */
	template<> inline double Atan<double>(double s, double c) noexcept { return atan2(s, c); }

	/*! \brief Distance between two angles
	 *
	 * Distance between two angles
	 *
	 * Usage example: AngularDistance<ByteAngle>(35, Angle<ByteAngle>::TOP)
	 *
	 * \param[in]	a1	first angle
	 * \param[in]	a2	second angle
	 */
	template<typename Unit> inline typename Unit::type AngularDistance(const Angle<Unit> &a1, const Angle<Unit> &a2) noexcept(
			std::is_nothrow_constructible<typename TypeInfo<typename Unit::type>::DiffType>::value && 
			std::is_nothrow_copy_assignable<typename TypeInfo<typename Unit::type>::DiffType>::value &&
			std::is_nothrow_constructible<typename Unit::type>::value)
	{
	// TODO what is the fastest? min(a-b, 2PI - (a-b)) or PI - abs(PI - abs(a-b))
		typename TypeInfo<typename Unit::type>::DiffType dist(Abs((typename TypeInfo<typename Unit::type>::DiffType)a1.value - (typename TypeInfo<typename Unit::type>::DiffType)a2.value));
		if (dist > Unit::MAXVAL / 2)
			dist = Unit::MAXVAL - dist;
		return typename Unit::type(dist);
	}
	/*! \brief Distance between two angles
	 *
	 * Distance between two angles
	 *
	 * \param[in]	a1	first angle
	 * \param[in]	a2	second angle
	 */
	inline double AngularDistance(double a1, double a2) noexcept
	{
		const auto dist = Abs(a1 - a2);
		return (dist > M_PI) ? (M_PI - dist) : dist;
	}

	/*! \brief Mean of a set of angles 
	 * \throws	ExceptionDomain	empty set of angles
	 * \param[in]	beg	iterator on the first angle
	 * \param[in]	en	iterator after the last angle
	 * \return	the mean angle
	 */
	template<typename ITER> typename std::iterator_traits<ITER>::value_type AngularMean(ITER beg, ITER en)
	{
		using return_type = typename std::iterator_traits<ITER>::value_type;
		if (beg == en)
			throw ExceptionDomain("AngularMean(): empty set of angles.");
		double s = 0, c = 0;
		for (ITER it = beg; it != en; ++it)
		{
			s += Sin(*it);
			c += Cos(*it);
		}
		return Atan<return_type>(s, c);
	}

	/*! \brief Mean of a set of angles 
	 * \throws	ExceptionDomain	empty set of angles
	 * \param[in]	beg	iterator on the first angle
	 * \param[in]	en	iterator after the last angle
	 * \return	an iterator to the median angle
	 */
	template<typename ITER> ITER AngularMedian(ITER beg, ITER en)
	{
		if (beg == en)
			throw ExceptionDomain("AngularMedian(): empty set of angles.");
		auto mdist = std::vector<double>(std::distance(beg, en), 0.0);
		auto cnt1 = size_t(0);
		for (auto it1 = beg; it1 != en; ++cnt1, ++it1)
		{
			auto cnt2 = cnt1 + 1;
			for (auto it2 = std::next(it1); it2 != en; ++cnt2, ++it2)
			{
				const auto d = AngularDistance(*it1, *it2);
				mdist[cnt1] += d;
				mdist[cnt2] += d;
			}
		}
		return std::next(beg, std::min_element(mdist.begin(), mdist.end()) - mdist.begin());
	}

	/*! \brief Variance of a set of angles 
	 * \throws	ExceptionDomain	empty set of angles
	 * \param[in]	beg	iterator on the first angle
	 * \param[in]	en	iterator after the last angle
	 * \return	the biased variance
	 */
	template<typename ITER> double AngularVariance(ITER beg, ITER en)
	{
		using angle_type = typename std::iterator_traits<ITER>::value_type;
		if (beg == en)
			throw ExceptionDomain("AngularVariance(): empty set of angles.");
		angle_type mean(AngularMean(beg, en));
		double acc = 0;
		for (ITER it = beg; it != en; ++it)
		{
			acc += Sqr(AngularDistance<typename angle_type::unit>(mean, *it));
		}
		return acc / double(std::distance(beg, en));
	}

	/*! \brief Variance of a set of angles 
	 * \throws	ExceptionDomain	empty set of angles
	 * \param[in]	beg	iterator on the first angle
	 * \param[in]	en	iterator after the last angle
	 * \param[in]	mean	the mean of the set of angles
	 * \return	the biased variance
	 */
	template<typename ITER> double AngularVariance(ITER beg, ITER en, typename std::iterator_traits<ITER>::value_type mean)
	{
		using angle_type = typename std::iterator_traits<ITER>::value_type;
		if (beg == en)
			throw ExceptionDomain("AngularVariance(): empty set of angles.");
		double acc = 0;
		for (ITER it = beg; it != en; ++it)
		{
			acc += Sqr(AngularDistance<typename angle_type::unit>(mean, *it));
		}
		return acc / double(std::distance(beg, en));
	}

	/*! \brief Circular (pseudo) variance of a set of angles
	 * \throws	ExceptionDomain	empty set of angles
	 * \param[in]	beg	iterator on the first angle
	 * \param[in]	en	iterator after the last angle
	 * \return	the "circular variance"
	 */
	template<typename ITER> double CircularVariance(ITER beg, ITER en)
	{
		if (beg == en)
			throw ExceptionDomain("CircularVariance(): empty set of angles.");
		auto c = 0.0, s = 0.0;
		for (ITER it = beg; it != en; ++it)
		{
			c += it->Cos();
			s += it->Sin();
		}
		auto n = double(std::distance(beg, en));
		return 1.0 - sqrt(Sqr(c / n) + Sqr(s / n));
	}

	/*! \brief Circular (pseudo) standard deviation of a set of angles
	 * \throws	ExceptionDomain	empty set of angles
	 * \param[in]	beg	iterator on the first angle
	 * \param[in]	en	iterator after the last angle
	 * \return	the "circular standard deviation"
	 */
	template<typename ITER> double CircularStdDev(ITER beg, ITER en)
	{
		return sqrt(-2.0 * log(1.0 - CircularVariance(beg, en)));
	}

	/*! \brief Trigonometric moment
	 * \throws	ExceptionDomain	empty set of angles
	 * \throws	ExceptionInvalidArgument	null order
	 * \param[in]	beg	iterator on the first angle
	 * \param[in]	en	iterator after the last angle
	 * \param[in]	refer	the reference angle (typically, the mean angle)
	 * \param[in]	p	the order of the moment (>= 1)
	 * \return	the pth trigonometric moment
	 */
	template<typename ITER> typename std::complex<double> TrigonometricMoment(ITER beg, ITER en, typename std::iterator_traits<ITER>::value_type refer, size_t p)
	{
		if (beg == en)
			throw ExceptionDomain("TrigonometricMoment(): empty set of angles.");
		if (p == 0)
			throw ExceptionInvalidArgument("TrigonometricMoment(): null order.");
		auto c = 0.0, s = 0.0;
		for (auto it = beg; it != en; ++it)
		{
			c += Cos(double(p) * (*it - refer));
			s += Sin(double(p) * (*it - refer));
		}
		const auto n = double(std::distance(beg, en));
		c /= n;
		s /= n;
		return std::complex<double>(c, s);
	}

	/*! \brief Skewness of a set of angles
	 * \throws	ExceptionDomain	empty set of angles
	 * \param[in]	beg	iterator on the first angle
	 * \param[in]	en	iterator after the last angle
	 * \return	the skewness of the set
	 */
	template<typename ITER> double AngularSkewness(ITER beg, ITER en)
	{
		if (beg == en)
			throw ExceptionDomain("AngularSkewness(): empty set of angles.");
		const auto m = AngularMean(beg, en);
		const auto m1 = TrigonometricMoment(beg, en, m, 1);
		const auto m2 = TrigonometricMoment(beg, en, m, 2);
		return std::abs(m2) * Sin(Angle<Radian>(std::arg(m2)) - 2 * m) / pow(1 - std::abs(m1), 1.5);
	}

	/*! \brief Kurtosis of a set of angles
	 * \throws	ExceptionDomain	empty set of angles
	 * \param[in]	beg	iterator on the first angle
	 * \param[in]	en	iterator after the last angle
	 * \return	the kurtosis of the set
	 */
	template<typename ITER> double AngularKurtosis(ITER beg, ITER en)
	{
		if (beg == en)
			throw ExceptionDomain("AngularKurtosis(): empty set of angles.");
		const auto m = AngularMean(beg, en);
		const auto m1 = TrigonometricMoment(beg, en, m, 1);
		const auto m2 = TrigonometricMoment(beg, en, m, 2);
		return (std::abs(m2) * Cos(Angle<Radian>(std::arg(m2)) - 2 * m) - pow(std::abs(m1), 4)) / Sqr(1 - std::abs(m1));
	}

	/*@}*/

}

#endif


