/* Copyright 2008-2016 INSA Lyon, CoReNum, Université Paris Descartes, ENS-Lyon
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
 * file: CRNRealToRealFunction.h
 * \author Jean DUONG, Yann LEYDIER
 */


#ifndef CRNREALTOREALFUNCTION_HEADER
#define CRNREALTOREALFUNCTION_HEADER

#include <CRNMath/CRNMatrixDouble.h>
#include <CRNMath/CRNRealToRealFunctionPtr.h>

namespace crn
{
	/****************************************************************************/
	/*! \brief Numerical integration toolbox class
	 *
	 * Tools for numerical integration
	 * 
	 * \author 	Jean DUONG
	 * \date	January 2009
	 * \version	0.2
	 * \ingroup math
	 */
	class RealToRealFunction: public Object
	{
		public:

			/*! \brief Default constructor */
			RealToRealFunction():left_endpoint(0),right_endpoint(0),image(nullptr) {}
			/*! \brief Constructor */
			RealToRealFunction(double l, double u);
			RealToRealFunction(const RealToRealFunction& F);
			RealToRealFunction(RealToRealFunction&&) = default;
			/*! \brief Clones the function */
			virtual UObject Clone() const override { return std::make_unique<RealToRealFunction>(*this); }

			/*! \brief Destructor */
			virtual ~RealToRealFunction() override;

			RealToRealFunction& operator=(const RealToRealFunction&) = delete;
			RealToRealFunction& operator=(RealToRealFunction&&) = default;

			/*! \brief Returns the beginning of the definition interval */
			double GetLeftEndpoint() const noexcept { return left_endpoint; }
			/*! \brief Returns the end of the definition interval */
			double GetRightEndpoint() const noexcept { return right_endpoint; }
			/*! \brief Returns the image of the function */
			SCMatrixDouble GetImage() const { return image; }
			/*! \brief Returns the number of elements modelized in the image */
			size_t GetImageSize() const noexcept { return image->GetCols(); }
			/*! \brief Returns an given image */
			double At(size_t k) const { return image->At(0, k); }

			/*! \brief Sets the beginning of the definition interval */
			void SetLeftEndpoint(double d) noexcept;
			/*! \brief Sets the end of the definition interval */
			void SetRightEndpoint(double d) noexcept;
			/*! \brief Sets a given image */
			void SetElement(size_t k, double v);
			/*! \brief Sets the number of elements to be modelized if the image */
			void SetImageSize(size_t s);
			/*! \brief Evaluates the image a given point */
			double Evaluate(double x) const;
			/*! \brief Finds the possible preimages of a given image value */
			MatrixDouble MakePreimage(double y) const;

			/*! \brief Evaluates the integral with the trapezoid method */
			double TrapezoidIntegral() const;

		private:
			SMatrixDouble image; /*!< line matrix */
			double left_endpoint; /*!< left extrema of the function */
			double right_endpoint; /*!< right extrema of the function */

	};

	namespace protocol
	{
		template<> struct IsClonable<RealToRealFunction> : public std::true_type {};
	}

}
#endif

