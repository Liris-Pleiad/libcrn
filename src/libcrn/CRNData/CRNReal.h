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

			/*! \brief This is a Serializable, Algebra, Field, POSet, Metric and Clonable object */
			virtual Protocol GetClassProtocols() const noexcept override { return crn::Protocol::Serializable|crn::Protocol::Algebra|crn::Protocol::Field|crn::Protocol::POSet|crn::Protocol::Metric|crn::Protocol::Clonable; }
			/*! \brief Returns the id of the class */
			virtual const String& GetClassName() const override { static const String cn(U"Real"); return cn; }

			using Object::Mult;
			/*! \brief External multiplication
			 * 
			 * External multiplication
			 * 
			 * \param[in]	m	the multiplication factor
			 */
			virtual void Mult(double m) override { val *= m; }

			/*! \brief Dumps value to a string */
			virtual String ToString() const override;
			/*! \brief Creates a new object this one */
			virtual UObject Clone() const override { return std::make_unique<Real>(val); }

			/*! \brief Returns the value */
			double GetValue() const noexcept { return val; }
			/*! \brief Converts to double */
			operator double() const noexcept { return val; }
			/*! \brief Sets the value */
			void SetValue(double d) noexcept { val = d; }

		private:
			double val; /*!< internal value storage */
			/*! \brief Distance between to reals */
			virtual double distance(const Object &obj) const override;
			/*! \brief Addition with another real */
			virtual void add(const Object &v) override { val += static_cast<const Real&>(v).GetValue(); }
			/*! \brief Subtraction with another real */
			virtual void sub(const Object &v) override { val -= static_cast<const Real&>(v).GetValue(); }
			/*! \brief Equality with another real */
			virtual bool equals(const Object &v) const override { if (val == static_cast<const Real&>(v).GetValue()) return true; else return false; };
			/*! \brief Sum of reals */
			virtual UObject sum(const std::vector<std::pair<const Object*, double> > &plist) const override;
			/*! \brief Mean of reals */
			virtual UObject mean(const std::vector<std::pair<const Object*, double> > &plist) const override;
			/*! \brief Multiplication with another real */
			virtual void mult(const Object &obj) override;
			/*! \brief Division with another real */
			virtual void div(const Object &obj) override;
			/*! \brief Comparison with another real */
			virtual Prop3 ge(const Object &l) const override;
			/*! \brief Comparison with another real */
			virtual Prop3 le(const Object &l) const override;

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
}

#include <CRNData/CRNRealPtr.h>

#endif
