/* Copyright 2011-2016 CoReNum, INSA-Lyon, Université Paris Descartes, ENS-Lyon
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
 * file: CRNPolynomialRegression.h
 * \author Yann LEYDIER
 */

#ifndef CRNPolynomialRegression_HEADER
#define CRNPolynomialRegression_HEADER

#include <CRNMath/CRNCartesian2DFunction.h>
#include <CRNGeometry/CRNPoint2DDouble.h>
#include <CRNException.h>

namespace crn
{
	/*! \brief Polynomial regression
	 *
	 * Creates a polynomial function representing a cloud of points.
	 *
	 * \version 1.0
	 * \author	Yann LEYDIER
	 * \date	Sep 2011
	 * \ingroup	math
	 */
	class PolynomialRegression: public crn::Cartesian2DFunction
	{
		public:
			/*! \brief Extrapolation modes */
			enum class Extrapolation { LINEAR, POLY };

			/*! \brief Constructor from a list of points 
			 * \throws	crn::ExceptionDomain	the order of the polynomial is null
			 * \throws	crn::ExceptionDimension	not enough samples
			 * \param[in]	beg	the first element
			 * \param[in]	en	the element after the last
			 * \param[in]	dim	the dimension of the regression polynomial
			 */
			template<typename Iter> PolynomialRegression(Iter beg, Iter en, size_t dim):
				dimension(dim),
				data(std::distance(beg, en)),
				coefficients(dim + 1),
				extrapolation(Extrapolation::POLY)
			{
				if (dim == 0)
					throw crn::ExceptionDomain("Null order");
				if (data.size() < dimension)
					throw crn::ExceptionDimension("There must be more samples than the dimension.");
				size_t cnt = 0;
				for (Iter tmp = beg; tmp != en; ++tmp)
					data[cnt++] = makePoint(*tmp);
				computeCoeffs();
			}
			PolynomialRegression(const PolynomialRegression&) = default;
			PolynomialRegression(PolynomialRegression&&) = default;
			PolynomialRegression& operator=(const PolynomialRegression&) = default;
			PolynomialRegression& operator=(PolynomialRegression&&) = default;
			virtual ~PolynomialRegression() override {}

			/*! \brief Sets the behaviour for points before the first control point and after the last control point */
			void SetExtrapolationMode(Extrapolation ex) noexcept { extrapolation = ex; }

			/*! \brief Translates the polynomial */
			void TranslateY(int increment);

			/*! \brief Gets ordinate at x (double) */
			virtual double operator[](double x) const override;
			/*! \brief Gets ordinate at x (int) */
			using Cartesian2DFunction::operator[];

			/*! \brief Returns the dimension of the polynomial */
			size_t GetDimension() const noexcept { return dimension; }
			/*! \brief Access to the sorted control point */
			const std::vector<crn::Point2DDouble>& GetData() const noexcept { return data; }
			/*! \brief Access to the coefficients of the polynomial */
			const std::vector<double>& GetCoefficients() const noexcept { return coefficients; }

		private:
			/*! \brief Converts any Point2D to Point2DDouble */
			template<typename T> Point2DDouble makePoint(const crn::Point2D<T> &p)
			{ return crn::Point2DDouble(double(p.X), double(p.Y)); }
			/*! \brief Converts any Point2D to Point2DDouble */
			template<typename T> Point2DDouble makePoint(const crn::Point2D<T> *p)
			{ return crn::Point2DDouble(double(p->X), double(p->Y)); }
			/*! \brief Converts any pair to Point2DDouble */
			template<typename T, typename Y> Point2DDouble makePoint(const std::pair<T, Y> &p)
			{ return crn::Point2DDouble(double(p.first), double(p.second)); }
			/*! \brief Computes the coefficients of the spline */
			void computeCoeffs();

			std::vector<double> coefficients;
			std::vector<crn::Point2DDouble> data; /*!< the points */
			Extrapolation extrapolation;
			size_t dimension;

			CRN_DECLARE_CLASS_CONSTRUCTOR(PolynomialRegression)
	};
	template<> struct IsClonable<PolynomialRegression> : public std::true_type {};

	CRN_ALIAS_SMART_PTR(PolynomialRegression)
}

#endif

