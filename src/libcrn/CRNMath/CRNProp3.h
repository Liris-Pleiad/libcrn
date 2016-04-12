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
 * file: CRNProp3.h
 * \author Yann LEYDIER
 */

#ifndef CRNPROP3_HEADER
#define CRNPROP3_HEADER

#include <CRNString.h>

#ifdef FALSE
#undef FALSE
#endif
#define FALSE 0
#ifdef TRUE
#undef TRUE
#endif
#define TRUE 1

namespace crn
{

	/****************************************************************************/
	/*! \brief A ternary proposition
	 *
	 * A class for handling ternary logic
	 *
	 * \author 	Yann LEYDIER
	 * \date		30 October 2006
	 * \version 0.1
	 * \ingroup math
	 */
	class Prop3: public Object
	{
		public:
			/*! \brief Default constructor */
			Prop3() noexcept : value(Prop3::UNKNOWN) {}
			/*! \brief Constructor from value */
			Prop3(int val) noexcept;
			constexpr Prop3(const Prop3 &) = default;
			constexpr Prop3(Prop3 &&) = default;
			~Prop3() = default;

			Prop3& operator=(const Prop3 &) = default;
			Prop3& operator=(Prop3 &&) = default;

			/*! \brief Logical OR operator */
			Prop3 operator|(const Prop3 &prop) const noexcept;
			/*! \brief Logical OR operator */
			Prop3& operator|=(const Prop3 &prop) noexcept;
			/*! \brief Logical AND operator */
			Prop3 operator&(const Prop3 &prop) const noexcept;
			/*! \brief Logical AND operator */
			Prop3& operator&=(const Prop3 &prop) noexcept;
			/*! \brief Comparison */
			bool operator==(const Prop3 &prop) const noexcept;
			/*! \brief Assignment operator */
			Prop3& operator=(const int &prop) noexcept;
			/*! \brief Complementary operator */
			Prop3 operator!() const noexcept;
			/*! \brief Is true? */
			bool IsTrue() const noexcept { if (value == TRUE) return true; else return false; }
			/*! \brief Is false? */
			bool IsFalse() const noexcept { if (value == FALSE) return true; else return false; }
			/*! \brief Is unknown? */
			bool IsUnknown() const noexcept { if (value == Prop3::UNKNOWN) return true; else return false; }

			/*! \brief Dumps value to a string */
			String ToString() const;
			/*! \brief Returns the internal integer value */
			int GetValue() const noexcept { return value; }

			static const int UNKNOWN; /*!< UNKNOWN value */
			static const Prop3 True; /*!< True constant */
			static const Prop3 False; /*!< False constant */
			static const Prop3 Unknown; /*!< Unknown constant */

			/*! \brief Initializes the object from an XML element. Unsafe. */
			void Deserialize(xml::Element &el);
			/*! \brief Dumps the object to an XML element. Unsafe. */
			xml::Element Serialize(xml::Element &parent) const;

		private:
			int value; /*!< internal value */

		CRN_DECLARE_CLASS_CONSTRUCTOR(Prop3)
		CRN_SERIALIZATION_CONSTRUCTOR(Prop3)
	};
	template<> struct IsSerializable<Prop3> : public std::true_type {};
	template<> struct IsClonable<Prop3> : public std::true_type {};

	CRN_ALIAS_SMART_PTR(Prop3)
}
#endif

