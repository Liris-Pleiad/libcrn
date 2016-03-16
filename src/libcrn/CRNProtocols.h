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

/*! \addtogroup base */
/*@{*/
namespace crn
{
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

