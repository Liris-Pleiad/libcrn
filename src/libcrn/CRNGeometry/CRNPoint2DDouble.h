/* Copyright 2008-2016 INSA Lyon, CoReNum, ENS-Lyon
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
 * file: CRNPoint2DDouble.h
 * \author Yann LEYDIER
 */

#ifndef CRNPOINT2DDOUBLE_HEADER
#define CRNPOINT2DDOUBLE_HEADER

#include <CRNGeometry/CRNPoint2D.h>

namespace crn
{
	/****************************************************************************/
	/*! \brief A 2D point class
	 *
	 * Convenience 2D point class.
	 *
	 * \author 	Yann LEYDIER
	 * \date		September 2008
	 * \version 0.1
	 * \ingroup geo
	 */
	class Point2DDouble: public Point2D<double>
	{
		public:
			/*! \brief Default constructor */
			Point2DDouble() noexcept:Point2D<double>() {}
			/*! \brief Constructor from values */
			Point2DDouble(double x, double y) noexcept:Point2D<double>(x,y) {}
			Point2DDouble(const Point2D<double> &p) noexcept:Point2D<double>(p) {}
			Point2DDouble(const Point2DDouble&) = default;
			Point2DDouble(Point2DDouble&&) = default;
			/*! \brief Destructor */
			virtual ~Point2DDouble() override {}
		
			Point2DDouble& operator=(const Point2DDouble&) = default;
			Point2DDouble& operator=(Point2DDouble&&) = default;

			/*! \brief Rotation for single point */
			Point2DDouble MakeRotation(Angle<Radian> theta) const;
		
			/*!\brief Creates a new object, copied from this */
			virtual UObject Clone() const override { return std::make_unique<Point2DDouble>(X, Y); }

		private:
			/*! \brief Initializes the object from an XML element. Unsafe. */
			virtual void deserialize(xml::Element &el) override;
			/*! \brief Dumps the object to an XML element. Unsafe. */
			virtual xml::Element serialize(xml::Element &parent) const override;

		CRN_DECLARE_CLASS_CONSTRUCTOR(Point2DDouble)
		CRN_SERIALIZATION_CONSTRUCTOR(Point2DDouble)
	};
	namespace protocol
	{
		template<> struct IsSerializable<Point2DDouble> : public std::true_type {};
		template<> struct IsClonable<Point2DDouble> : public std::true_type {};
	}

	CRN_ALIAS_SMART_PTR(Point2DDouble)
}

#endif
