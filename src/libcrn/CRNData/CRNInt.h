/* Copyright 2009-2014 INSA Lyon, CoReNum
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

			/*! \brief This is a Serializable, Ring, POSet, Metric and Clonable object */
			virtual Protocol GetClassProtocols() const noexcept override { return crn::Protocol::Serializable|crn::Protocol::Ring|crn::Protocol::POSet|crn::Protocol::Metric|crn::Protocol::Clonable; }
			/*! \brief Returns the id of the class */
			virtual const String& GetClassName() const override { static const String cn(U"Int"); return cn; }

			/*! \brief Dumps value to a string */
			virtual String ToString() const override { return String(val); }
			/*! \brief Creates a new object this one */
			virtual UObject Clone() const override { return std::make_unique<Int>(val); }

			/*! \brief Returns the value */
			int GetValue() const noexcept { return val; }
			/*! \brief Converts to double */
			operator int() const noexcept { return val; }
			/*! \brief Sets the value */
			void SetValue(int i) noexcept { val = i; }

		private:
			int val; /*!< internal value storage */
			/*! \brief Distance between to reals */
			virtual double distance(const Object &obj) const override { return Abs(val - static_cast<const Int&>(obj).GetValue()); }
			/*! \brief Addition with another real */
			virtual void add(const Object &v) override { val += static_cast<const Int&>(v).GetValue(); }
			/*! \brief Subtraction with another real */
			virtual void sub(const Object &v) override { val -= static_cast<const Int&>(v).GetValue(); }
			/*! \brief Equality with another real */
			virtual bool equals(const Object &v) const override { if (val == static_cast<const Int&>(v).GetValue()) return true; else return false; };
			/*! \brief Multiplication with another real */
			virtual void mult(const Object &obj) override { val *= static_cast<const Int&>(obj).GetValue(); }
			/*! \brief Comparison with another real */
			virtual Prop3 ge(const Object &l) const override { return val >= static_cast<const Int&>(l).GetValue(); }
			/*! \brief Comparison with another real */
			virtual Prop3 le(const Object &l) const override { return val <= static_cast<const Int&>(l).GetValue(); }

			/*! \brief Reads from an XML element */
			virtual void deserialize(xml::Element &el) override;
			/*! \brief Dumps to an XML element */
			virtual xml::Element serialize(xml::Element &parent) const override;

		CRN_DECLARE_CLASS_CONSTRUCTOR(Int)
		CRN_SERIALIZATION_CONSTRUCTOR(Int)
	};
}

#include <CRNData/CRNIntPtr.h>

#endif


