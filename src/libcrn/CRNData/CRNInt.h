/* Copyright 2009-2016 INSA Lyon, CoReNum, ENS-Lyon
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
 * file: CRNInt.h
 * \author Yann LEYDIER
 */

#ifndef CRNInt_HEADER
#define CRNInt_HEADER

#include <CRNObject.h>
#include <CRNString.h>
#include <CRNMath/CRNProp3.h>
#include <CRNMath/CRNMath.h>

namespace crn
{
	/****************************************************************************/
	/*! \brief Interface class for the metric real number class
	 *
	 * \author 	Yann LEYDIER
	 * \date		September 2009
	 * \version 0.1
	 * \ingroup data
	 */
	class Int: public Object
	{
		public:
			/*! \brief Default constructor*/
			Int(int i = 0) noexcept:val(i) {}
			/*! \brief Destructor*/
			virtual ~Int() override {}

			Int(const Int&) = default;
			Int(Int&&) = default;
			Int& operator=(const Int&) = default;
			Int& operator=(Int&&) = default;

			/*! \brief Dumps value to a string */
			virtual String ToString() const override { return String(val); }
			/*! \brief Creates a new object this one */
			virtual UObject Clone() const override { return std::make_unique<Int>(val); }

			/*! \brief Converts to int */
			operator int() const noexcept { return val; }

			Int& operator+=(Int i) noexcept { val += i.val; return *this; }
			Int& operator-=(Int i) noexcept { val -= i.val; return *this; }
			Int& operator*=(Int i) noexcept { val *= i.val; return *this; }
			Int& operator/=(Int i) noexcept { val /= i.val; return *this; }
			bool operator<(Int i) noexcept { return val < i.val; }
			bool operator>(Int i) noexcept { return val > i.val; }
			bool operator<=(Int i) noexcept { return val <= i.val; }
			bool operator>=(Int i) noexcept { return val >= i.val; }

		private:
			int val; /*!< internal value storage */
			/*! \brief Reads from an XML element */
			virtual void deserialize(xml::Element &el) override;
			/*! \brief Dumps to an XML element */
			virtual xml::Element serialize(xml::Element &parent) const override;

		CRN_DECLARE_CLASS_CONSTRUCTOR(Int)
		CRN_SERIALIZATION_CONSTRUCTOR(Int)
	};
	namespace protocol
	{
		template<> struct IsSerializable<Int> : public std::true_type {};
	}
}
inline crn::Int operator+(crn::Int i1, const crn::Int &i2) noexcept { return i1 += i2; }
inline crn::Int operator-(crn::Int i1, const crn::Int &i2) noexcept { return i1 -= i2; }
inline crn::Int operator*(crn::Int i1, const crn::Int &i2) noexcept { return i1 *= i2; }
inline double operator*(double d, const crn::Int &i) noexcept { return d * int(i); }
inline double operator*(const crn::Int &i, double d) noexcept { return d * int(i); }
inline crn::Int operator/(crn::Int i1, const crn::Int &i2) noexcept { return i1 /= i2; }

#include <CRNData/CRNIntPtr.h>

#endif


