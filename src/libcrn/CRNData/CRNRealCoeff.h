/* Copyright 2007-2014 Yann LEYDIER, CoReNum, INSA-Lyon
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
 * file: CRNRealCoeff.h
 * \author Yann LEYDIER
 */

#ifndef CRNREALCOEFF_HEADER
#define CRNREALCOEFF_HEADER

#include <CRNData/CRNReal.h>

namespace crn
{
	/***************************************************************************/
	/*! \brief Interface class for the metric real number with a coefficient
	 *
	 * \author 	Yann LEYDIER
	 * \date		June 2007
	 * \version 0.2
	 * \ingroup data
	 */
	class RealCoeff: public Real
	{
		public:
			/*! \brief Default constructor
			 * 
			 * Default constructor
			 * 
			 * \param[in]	d	the value
			 * \param[in]	c	the coefficient
			 */
			RealCoeff(double d = 0, double c = 1):Real(d),coeff(c) {}
			/*! \brief Destructor */
			virtual ~RealCoeff() override {}

			RealCoeff(const RealCoeff&) = default;
			RealCoeff(RealCoeff&&) = default;
			RealCoeff& operator=(const RealCoeff&) = default;
			RealCoeff& operator=(RealCoeff&&) = default;

			/*! \brief Dumps value to a string */
			virtual String ToString() const override;
			/*! \brief Creates a new object this one */
			virtual UObject Clone() const override { return std::make_unique<RealCoeff>(GetValue(), coeff); }
			/*! \brief Returns the id of the class */
			virtual const String& GetClassName() const override { static const String cn(U"RealCoeff"); return cn; }

			/*! \brief Returns the coefficient */
			double GetCoeff() const noexcept { return coeff; }
			/*! \brief Sets the coefficient */
			void SetCoeff(double d) noexcept { coeff = d; }

		private:
			double coeff; /*!< internal coefficient storage */
			// distance: same as Real::distance --> coeff ignored
			/*! \brief Addition with another real */
			virtual void add(const Object &v) override;
			/*! \brief Subtraction with another real */
			virtual void sub(const Object &v) override;
			// equals : same as Real::equals. --> coeff ignored
			/*! \brief Sum of reals */
			virtual UObject sum(const std::vector<std::pair<const Object*, double> > &plist) const override;
			/*! \brief Mean of reals */
			virtual UObject mean(const std::vector<std::pair<const Object*, double> > &plist) const override;
			/*! \brief Multiplication with another real */
			virtual void mult(const Object &obj) override;
			/*! \brief Division with another real */
			virtual void div(const Object &obj) override;
			// ge/le : same as Real::ge/le. --> coeff ignored

			/*! \brief Reads from an XML element */
			virtual void deserialize(xml::Element &el) override;
			/*! \brief Dumps to an XML element */
			virtual xml::Element serialize(xml::Element &parent) const override;

		CRN_DECLARE_CLASS_CONSTRUCTOR(RealCoeff)
		CRN_SERIALIZATION_CONSTRUCTOR(RealCoeff)
	};

	CRN_ALIAS_SMART_PTR(RealCoeff)
}
#endif
