/* Copyright 2006-2016 Yann LEYDIER, INSA-Lyon, ENS-Lyon
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
 * file: CRNProtocols.h
 * \author Yann LEYDIER, Jean DUONG
 */

#ifndef CRNPROTOCOLS
#define CRNPROTOCOLS

#include <CRNType.h>
#include <CRNMath/CRNMath.h>

/*! \addtogroup base */
/*@{*/
namespace crn
{
	// Metric objects
	
	/*! \brief Distance between two objects */
	template<typename T> double Distance(const T &o1, const T &o2, typename std::enable_if<std::is_class<T>::value>::type *dummy = nullptr)
	{
		return o1.Distance(o2);
	}
	/*! \brief Distance between two integral numbers */
	template<typename T> double Distance(T o1, T o2, typename std::enable_if<std::is_arithmetic<T>::value>::type *dummy = nullptr)
	{
		return double(Abs(o1 - o2));
	}
	/*! Has:
	 * - Distance(T, T)
	 */
	template<typename T> struct IsMetric: public std::integral_constant<bool, std::is_arithmetic<T>::value>{};

	// Partially ordered objects
	namespace impl
	{
		struct Dummy {};
		template<typename T> Dummy operator<(const T &, const T &) { return Dummy{}; }
		template<typename T> auto LT(const T &o1, const T &o2) -> decltype(o1 < o2) { return o1 < o2; }
	}
	template<typename T> struct HasLT :
		public std::integral_constant<
		bool,
		!std::is_same<
		impl::Dummy,
		typename std::result_of<decltype(impl::LT<T>)&(const T &, const T &)>::type
		>::value
		>
	{};

	/*! Has:
	 * - Prop3 operator<(T, T)
	 * - Prop3 operator<=(T, T)
	 * - Prop3 operator>(T, T)
	 * - Prop3 operator>=(T, T)
	 */
	template<typename T> struct IsPOSet: public std::integral_constant<bool, std::is_arithmetic<T>::value> {};

	// Addable objects
	
	/*! Has:
	 * - operator+(T, T)
	 */
	template<typename T> struct IsMagma: public std::integral_constant<bool, std::is_arithmetic<T>::value> {};

	// Addable and subtractable objects
	
	/*! Has:
	 * - operator+(T, T)
	 * - operator-(T, T)
	 */
	template<typename T> struct IsGroup: public std::integral_constant<bool, std::is_arithmetic<T>::value> {};

	// Addable, subtractable and inner-multipliable objects
	
	/*! Has:
	 * - operator+(T, T)
	 * - operator-(T, T)
	 * - operator*(T, T)
	 */
	template<typename T> struct IsRing: public std::integral_constant<bool, std::is_arithmetic<T>::value> {};

	// Addable, subtractable and outer-multipliable objects
	
	/*! Has:
	 * - operator+(T, T)
	 * - operator-(T, T)
	 * - operator*(T, double)
	 * - operator*(double, T)
	 */
	template<typename T> struct IsVectorOverR: public std::integral_constant<bool, std::is_arithmetic<T>::value> {};

	// Addable, subtractable, inner-multipliable and outer-multipliable objects
	
	/*! Has:
	 * - operator+(T, T)
	 * - operator-(T, T)
	 * - operator*(T, T)
	 * - operator*(T, double)
	 * - operator*(double, T)
	 */
	template<typename T> struct IsAlgebra: public std::integral_constant<bool, std::is_arithmetic<T>::value> {};

	// Addable, subtractable, inner-multipliable, outer-multipliable and dividable objects
	
	/*! Has:
	 * - operator+(T, T)
	 * - operator-(T, T)
	 * - operator*(T, T)
	 * - operator*(T, double)
	 * - operator*(double, T)
	 * - operator/(T, T)
	 */
	template<typename T> struct IsField: public std::integral_constant<bool, std::is_arithmetic<T>::value> {};

	// Serializable objects
	
	/*! Has:
	 * - T::T(xml::Element &)
	 * - Serialize(const T &, xml::Element &parent)
	 * - Deserialize(T &, xml::Element &)
	 */
	template<typename T> struct IsSerializable: public std::false_type {};

	// Clonable objects
	
	/*! Has:
	 * - T::Clone()
	 */
	template<typename T> struct IsClonable: public std::false_type {};

	// Savable objects
	
	/*! Has:
	 * - T::T(const Path &)
	 * - Save(const T &, const Path &)
	 * - Load(T &, const Path &)
	 */
	template<typename T> struct IsSavable: public std::false_type {};

	enum class Protocol: uint32_t { 
		// *** Base classes
		// -> Object
		Object = 1 << 30,
		// with distance
		Metric = (1<<29)|Object,
		// with order
		POSet = (1<<28)|Object,
		// with addition
		Magma = (1<<27)|Object,
		// with subtraction
		Group = (1<<26)|Magma,
		// with internal product
		Ring = (1<<25)|Group,
		// with external product
		VectorOverR = (1<<24)|Group,
		// with both products
		Algebra = Ring|VectorOverR,
		// with division
		Field = (1<<23)|Ring,
		// with serialization (is also savable IF complex)
		Serializable = (1<<22)|(1<<19)|Object,
		// with cloning
		Clonable = (1<<21)|Object,

		// -> ComplexObject
		ComplexObject = (1<<20)|Object,
		// with file saving
		Savable = (1<<19)|ComplexObject,

		// -> Pixel
		Pixel = (1<<18)|Clonable|Metric|VectorOverR|POSet|Serializable,
		// -> Image
		Image = (1<<17)|Clonable|ComplexObject,

		// *** Composed virtual categories
		// -> Feature
		Feature = Serializable|Metric|Clonable
	};
}
CRN_DECLARE_ENUM_OPERATORS(crn::Protocol)
/*@}*/
#endif

