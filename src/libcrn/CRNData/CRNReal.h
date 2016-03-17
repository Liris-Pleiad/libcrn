/* Copyright 2006-2016 Yann LEYDIER, CoReNum, INSA-Lyon, ENS-Lyon
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
 * file: CRNReal.h
 * \author Yann LEYDIER
 */

#ifndef CRNREAL_HEADER
#define CRNREAL_HEADER

#include <CRNObject.h>
#include <CRNString.h>

namespace crn
{
	/****************************************************************************/
	/*! \brief Interface class for the metric real number class
	 *
	 * \author 	Yann LEYDIER
	 * \date		March 2007
	 * \version 0.3
	 * \ingroup data
	 */
	class Real: public Object
	{
		public:
			/*! \brief Default constructor*/
			Real(double d = 0):val(d) {}
			/*! \brief Destructor*/
			virtual ~Real() override {}

			Real(const Real&) = default;
			Real(Real&&) = default;
			Real& operator=(const Real&) = default;
			Real& operator=(Real&&) = default;

			/*! \brief Dumps value to a string */
			virtual String ToString() const override;
			/*! \brief Creates a new object this one */
			virtual UObject Clone() const override { return std::make_unique<Real>(val); }

			/*! \brief Converts to double */
			operator double() const noexcept { return val; }

			Real& operator+=(Real r) noexcept { val += r.val; return *this; }
			Real& operator-=(Real r) noexcept { val -= r.val; return *this; }
			Real& operator*=(Real r) noexcept { val *= r.val; return *this; }
			Real& operator/=(Real r) noexcept { val /= r.val; return *this; }
			bool operator<(Real r) noexcept { return val < r.val; }
			bool operator>(Real r) noexcept { return val > r.val; }
			bool operator<=(Real r) noexcept { return val <= r.val; }
			bool operator>=(Real r) noexcept { return val >= r.val; }

		private:
			double val; /*!< internal value storage */

			/*! \brief Reads from an XML element */
			virtual void deserialize(xml::Element &el) override;
			/*! \brief Dumps to an XML element */
			virtual xml::Element serialize(xml::Element &parent) const override;

		CRN_DECLARE_CLASS_CONSTRUCTOR(Real)
		CRN_SERIALIZATION_CONSTRUCTOR(Real)
	};
	namespace protocol
	{
		template<> struct IsSerializable<Real> : public std::true_type {};
	}
	inline double Distance(const Real &r1, const Real &r2) noexcept { return Abs(r1 - r2); }
}
inline crn::Real operator+(crn::Real r1, const crn::Real &r2) noexcept { return r1 += r2; }
inline crn::Real operator-(crn::Real r1, const crn::Real &r2) noexcept { return r1 -= r2; }
inline crn::Real operator*(crn::Real r1, const crn::Real &r2) noexcept { return r1 *= r2; }
inline crn::Real operator/(crn::Real r1, const crn::Real &r2) noexcept { return r1 /= r2; }

#include <CRNData/CRNRealPtr.h>

#endif
