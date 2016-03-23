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
 * file: CRNStroke.h
 * \author Yann LEYDIER
 */

#ifndef CRNSTROKE_HEADER
#define CRNSTROKE_HEADER

#include <CRNObject.h>
#include <CRNData/CRNVector.h>
#include <CRNGeometry/CRNPoint2DInt.h>
#include <CRNGeometry/CRNRect.h>

namespace crn
{
	class Stroke;
	CRN_ALIAS_SMART_PTR(Stroke)
}
namespace crn
{
	/****************************************************************************/
	/*! \brief A list of points.
	 *
	 * A stroke that is an ordered set of Point2DInt.
	 *
	 * \author 	Yann LEYDIER
	 * \date		November 2008
	 * \version 0.1
	 * \ingroup geo
	 */
	class Stroke: public Object
	{
		public:
			using datatype = std::vector<Point2DInt>;

			/*! \brief Default constructor */
			Stroke();
			/*! \brief Destructor */
			virtual ~Stroke() override;

			Stroke(const Stroke&) = delete;
			Stroke& operator=(const Stroke&) = delete;
			Stroke(Stroke&&) = default;
			Stroke& operator=(Stroke&&) = default;

			/*! \brief Adds a point to the stroke. */
			void AddPoint(const Point2DInt &p);

			/*! \brief Adds a batch of points directly at the end of the stroke. */
			void MergeAtEnd(const Stroke &str);
			/*! \brief Adds a batch of points at their original position and adds points to interpolate. */
			void MergeWithXInterpolation(const Stroke &str, unsigned int step = 1);

			/*! \brief Returns the number of points in the stroke */
			size_t GetNbPoints() const noexcept { return points.size(); }
			/*! \brief Returns a reference to a point. */
			Point2DInt& GetPoint(size_t index);
			/*! \brief Returns a reference to a point. */
			const Point2DInt& GetPoint(size_t index) const;
			/*! \brief Returns a reference to a point (fast and unsafe). */
			Point2DInt& operator[](size_t index) { return points[index]; }
			/*! \brief Returns a reference to a point (fast and unsafe). */
			const Point2DInt& operator[](size_t index) const { return points[index]; }

			/*! \brief Returns the ordinate of the first point with abscissa = x */
			double GetFirstYAtX(double x) const;

			/*! \brief Returns the bounding box using integer values. */
			Rect GetBBox() const;

			/*! \brief Creates a new stroke that is the intersection of the stroke with a rectangle */
			Stroke MakeIntersection(const Rect &rect) const;
			/*! \brief Creates a new stroke that is the first intersection of the stroke with a rectangle */
			Stroke MakeFirstIntersection(const Rect &rect) const;

			using iterator = datatype::iterator;
			/*! \brief Returns a new iterator on the first point of the stroke. */
			iterator begin() { return points.begin(); } 
			/*! \brief Returns a new iterator after the last point of the stroke. */
			iterator end() { return points.end(); } 
			using const_iterator = datatype::const_iterator;
			/*! \brief Returns a new const_iterator on the first point of the stroke. */
			const_iterator begin() const { return points.begin(); } 
			/*! \brief Returns a new const_iterator after the last point of the stroke. */
			const_iterator end() const { return points.end(); } 
			/*! \brief Returns a new const_iterator on the first point of the stroke. */
			const_iterator cbegin() const { return points.cbegin(); } 
			/*! \brief Returns a new const_iterator after the last point of the stroke. */
			const_iterator cend() const { return points.cend(); } 
			
			/*! \brief Initializes the object from an XML element. */
			void Deserialize(xml::Element &el);
			/*! \brief Dumps the object to an XML element. */
			xml::Element Serialize(xml::Element &parent) const;
			
		private:
			datatype points; /*!< Internal points storage */
			mutable SRect bbox; /*!< Temporary bounding box */

		CRN_DECLARE_CLASS_CONSTRUCTOR(Stroke)
		CRN_SERIALIZATION_CONSTRUCTOR(Stroke)
	};
	template<> struct IsSerializable<Stroke> : public std::true_type {};
	template<> struct IsClonable<Stroke> : public std::true_type {};
	UObject Clone(const Stroke &str);

	/*! \brief Size of a stroke */
	inline size_t Size(const Stroke &s) noexcept { return s.GetNbPoints(); }
}
CRN_ADD_RANGED_FOR_TO_POINTERS(crn::Stroke)
CRN_ADD_RANGED_FOR_TO_CONST_POINTERS(crn::Stroke)

#endif
