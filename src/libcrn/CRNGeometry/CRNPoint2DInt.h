/* Copyright 2008-2014 INSA Lyon, CoReNum
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
 * file: CRNPoint2DInt.h
 * \author Yann LEYDIER
 */

#ifndef CRNPOINT2DINT_HEADER
#define CRNPOINT2DINT_HEADER

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
	class Point2DInt: public Point2D<int>
	{
		public:
			/*! \brief Default constructor */
			Point2DInt() noexcept:Point2D<int>() {}
			/*! \brief Constructor from values */
			Point2DInt(int x, int y) noexcept:Point2D<int>(x,y) {}
			Point2DInt(const Point2D<int> &p) noexcept:Point2D<int>(p) {}
			Point2DInt(const Point2DInt &) = default;
			Point2DInt(Point2DInt &&) = default;
			/*! \brief Destructor */
			virtual ~Point2DInt() override {}

			Point2DInt& operator=(const Point2DInt &) = default;
			Point2DInt& operator=(Point2DInt &&) = default;

			/*! \brief This is a Metric, VectorOverR, Serializable and Clonable object */
			virtual Protocol GetClassProtocols() const noexcept override { return Point2D<int>::GetClassProtocols()|crn::Protocol::Serializable|crn::Protocol::Clonable; } 
			/*! \brief Returns the id of the class */
			virtual const String& GetClassName() const override { static const String cn(U"Point2DInt"); return cn; }
			
			/*! \brief Creates a new object, copied from this */
			virtual UObject Clone() const override { return std::make_unique<Point2DInt>(X, Y); }

			/*! \brief Moves the point towards a direction */
			void Advance(const Direction &dir, int step = 1);

		private:
			/*! \brief Initializes the object from an XML element. Unsafe. */
			virtual void deserialize(xml::Element &el) override;
			/*! \brief Dumps the object to an XML element. Unsafe. */
			virtual xml::Element serialize(xml::Element &parent) const override;

		CRN_DECLARE_CLASS_CONSTRUCTOR(Point2DInt)
		CRN_SERIALIZATION_CONSTRUCTOR(Point2DInt)
	};
  
	CRN_ALIAS_SMART_PTR(Point2DInt)

	/*! \brief Make a segment between two points */
	std::vector<Point2DInt> MakeSegment(const Point2DInt &p_begin, const Point2DInt &p_end);

}

#endif

