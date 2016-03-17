/* Copyright 2011-2015 CoReNum, INSA-Lyon, Université Paris Descartes
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
 * file: CRNCubicSpline.h
 * \author Yann LEYDIER
 */

#ifndef CRNCubicSpline_HEADER
#define CRNCubicSpline_HEADER

#include <CRNMath/CRNCartesian2DFunction.h>
#include <CRNGeometry/CRNPoint2DDouble.h>
#include <CRNException.h>

namespace crn
{
	/*! \brief Cubic spline interpolation
	 *
	 * Creates a cubic spline model representing a cloud of points.
	 *
	 * \version 1.0
	 * \author	Yann LEYDIER
	 * \date	Sep 2011
	 * \ingroup	math
	 */
	class CubicSpline: public Cartesian2DFunction
	{
		public:
			/*! \brief Extrapolation modes */
			enum class Extrapolation { LINEAR, CUBIC };

			/*! \brief Constructor from a list of points 
			 * \throws	crn::ExceptionDimension	not enough samples
			 * \param[in]	beg	the first element
			 * \param[in]	en	the element after the last
			 */
			template<typename Iter> CubicSpline(Iter beg, Iter en):
				data(std::distance(beg, en)),
				extrapolation(Extrapolation::CUBIC)
			{
				if (data.size() < 3)
					throw crn::ExceptionDimension("There must be at least 3 samples to create a spline");
				size_t cnt = 0;
				for (Iter tmp = beg; tmp != en; ++tmp)
					data[cnt++] = makePoint(*tmp);
				computeCoeffs();
			}
			CubicSpline(const CubicSpline&) = default;
			CubicSpline(CubicSpline&&) = default;
			virtual ~CubicSpline() override {}
			CubicSpline& operator=(const CubicSpline&) = default;
			CubicSpline& operator=(CubicSpline&&) = default;

			virtual UObject Clone() const override { return std::make_unique<CubicSpline>(*this); }

			/*! \brief Sets the behaviour for points before the first control point and after the last control point */
			void SetExtrapolationMode(Extrapolation ex) noexcept { extrapolation = ex; }

			/*! \brief Gets ordinate at x */
			virtual double operator[](double x) const override;
			/*! \brief Gets ordinate at x */
			using Cartesian2DFunction::operator[];

			/*! \brief Access to the sorted control point */
			const std::vector<crn::Point2DDouble>& GetData() const noexcept { return data; }

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

			std::vector<double> z; /*!< the coefficients */
			std::vector<crn::Point2DDouble> data; /*!< the points */
			Extrapolation extrapolation; /*!< the extrapolation method */
	};

	CRN_ALIAS_SMART_PTR(CubicSpline)

	namespace protocol
	{
		template<> struct IsClonable<CubicSpline> : public std::true_type {};
	}
}

#endif

