/* Copyright 2012-2016 CoReNum, ENS-Lyon
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
 * file: CRNLinearInterpolation.h
 * \author Yann LEYDIER
 */

#ifndef CRNLinearInterpolation_HEADER
#define CRNLinearInterpolation_HEADER

#include <CRNMath/CRNCartesian2DFunction.h>
#include <CRNGeometry/CRNPoint2DDouble.h>
#include <CRNException.h>

namespace crn
{
	/*! \brief Linear interpolation
	 *
	 * Creates a linear interpolation model representing a cloud of points.
	 *
	 * \version 1.0
	 * \author	Yann LEYDIER
	 * \date	Jan 2012
	 * \ingroup	math
	 */
	class LinearInterpolation: public crn::Cartesian2DFunction
	{
		public:
			/*! \brief Constructor from a list of points 
			 * \throws	crn::ExceptionDimension	not enough samples
			 * \param[in]	beg	the first element
			 * \param[in]	en	the element after the last
			 */
			template<typename Iter> LinearInterpolation(Iter beg, Iter en):
				data(std::distance(beg, en))
			{
				if (data.size() < 2)
					throw crn::ExceptionDimension("There must be at least 2 samples to interpolate anything.");
				size_t cnt = 0;
				for (Iter tmp = beg; tmp != en; ++tmp)
					data[cnt++] = makePoint(*tmp);
				std::sort(data.begin(), data.end(), Point2DDouble::Comparer(Direction::LEFT));
			}
			LinearInterpolation(const LinearInterpolation&) = default;
			LinearInterpolation(LinearInterpolation&&) = default;
			virtual ~LinearInterpolation() override {}
			LinearInterpolation& operator=(const LinearInterpolation&) = default;
			LinearInterpolation& operator=(LinearInterpolation&&) = default;

			/*! \brief Gets ordinate at x */
			virtual double operator[](double x) const override;
			/*! \brief Gets ordinate at x */
			using Cartesian2DFunction::operator[];

			/*! \brief Access to the sorted control point */
			const std::vector<crn::Point2DDouble>& GetData() const noexcept { return data; }

			void Deserialize(xml::Element &el);
			xml::Element Serialize(xml::Element &parent) const;

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
			
			std::vector<crn::Point2DDouble> data; /*!< the points */

		CRN_DECLARE_CLASS_CONSTRUCTOR(LinearInterpolation)			
		CRN_SERIALIZATION_CONSTRUCTOR(LinearInterpolation)			
	};
	template<> struct IsSerializable<LinearInterpolation> : public std::true_type {};
	template<> struct IsClonable<LinearInterpolation> : public std::true_type {};

	CRN_ALIAS_SMART_PTR(LinearInterpolation)
}

#endif

