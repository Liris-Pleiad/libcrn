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
			/*! \brief Default constructor */
			Stroke();
			/*! \brief Destructor */
			virtual ~Stroke() override;

			Stroke(const Stroke&) = delete;
			Stroke& operator=(const Stroke&) = delete;
			Stroke(Stroke&&) = default;
			Stroke& operator=(Stroke&&) = default;

			/*! \brief Converts object to string. */
			virtual String ToString() const override;

			/*! \brief Creates a new object, copied from this. */
			virtual UObject Clone() const override;

			/*! \brief Adds a point to the stroke. */
			void AddPoint(const Point2DInt &p);
			/*! \brief Adds a point to the stroke keeping reference to the object. */
			void AddAndKeepPoint(const SPoint2DInt &p);

			/*! \brief Adds a batch of points directly at the end of the stroke. */
			void MergeAtEnd(const Stroke &str);
			/*! \brief Adds a batch of points at their original position and adds points to interpolate. */
			void MergeWithXInterpolation(const Stroke &str, unsigned int step = 1);

			/*! \brief Returns the number of points in the stroke */
			size_t GetNbPoints() const noexcept { return points->Size(); }
			/*! \brief Returns a reference to a point. */
			SPoint2DInt GetPoint(size_t index);
			/*! \brief Returns a reference to a point. */
			SPoint2DInt GetPoint(size_t index) const;
			/*! \brief Returns a reference to a point (fast and unsafe). */
			SPoint2DInt operator[](size_t index) { return std::static_pointer_cast<Point2DInt>((*points)[index]); }
			/*! \brief Returns a reference to a point (fast and unsafe). */
			SCPoint2DInt operator[](size_t index) const { return std::static_pointer_cast<const Point2DInt>((*points)[index]); }

			/*! \brief Returns the ordinate of the first point with abscissa = x */
			double GetFirstYAtX(double x) const;

			/*! \brief Returns the bounding box using integer values. */
			Rect GetBBox() const;

			/*! \brief Creates a new stroke that is the intersection of the stroke with a rectangle */
			UStroke MakeIntersection(const Rect &rect) const;
			/*! \brief Creates a new stroke that is the first intersection of the stroke with a rectangle */
			UStroke MakeFirstIntersection(const Rect &rect) const;

			/*! \brief iterator on the points */
			class iterator: public std::iterator<std::input_iterator_tag, SPoint2DInt>
			{
				public:
					iterator(const iterator &other):it(other.it) {}
					iterator(const Vector::iterator &other):it(other) {}
					~iterator() {}
					iterator& operator=(const iterator &other) { it = other.it; return *this; }
					const iterator& operator++() { ++it; return *this; }
					iterator operator++(int nouse) { iterator temp = *this; ++it; return temp; }
					bool operator==(const iterator &other) { return it == other.it; }
					bool operator!=(const iterator &other) { return it != other.it; }
					value_type operator*() { return std::static_pointer_cast<Point2DInt>(*it); }
					value_type operator->() { return std::static_pointer_cast<Point2DInt>(*it); }
					
				private:
					Vector::iterator it;
			};
			/*! \brief const_iterator on the points */
			class const_iterator: public std::iterator<std::input_iterator_tag, SCPoint2DInt>
			{
				public:
					const_iterator(const const_iterator &other):it(other.it) {}
					const_iterator(const Vector::const_iterator &other):it(other) {}
					~const_iterator() {}
					const_iterator& operator=(const const_iterator &other) { it = other.it; return *this; }
					const const_iterator& operator++() { ++it; return *this; }
					const_iterator operator++(int nouse) { const_iterator temp = *this; ++it; return temp; }
					bool operator==(const const_iterator &other) { return it == other.it; }
					bool operator!=(const const_iterator &other) { return it != other.it; }
					value_type operator*() { return std::static_pointer_cast<const Point2DInt>(*it); }
					value_type operator->() { return std::static_pointer_cast<const Point2DInt>(*it); }
					
				private:
					Vector::const_iterator it;
			};

			/*! \brief Returns a new iterator on the first point of the stroke. */
			iterator begin() { return iterator(points->begin()); } 
			/*! \brief Returns a new iterator after the last point of the stroke. */
			iterator end() { return iterator(points->end()); } 
			/*! \brief Returns a new const_iterator on the first point of the stroke. */
			const_iterator begin() const { return const_iterator(points->begin()); } 
			/*! \brief Returns a new const_iterator after the last point of the stroke. */
			const_iterator end() const { return const_iterator(points->end()); } 
			/*! \brief Returns a new const_iterator on the first point of the stroke. */
			const_iterator cbegin() const { return const_iterator(points->cbegin()); } 
			/*! \brief Returns a new const_iterator after the last point of the stroke. */
			const_iterator cend() const { return const_iterator(points->cend()); } 
			
		private:
			/*! \brief Initializes the object from an XML element. Unsafe. */
			virtual void deserialize(xml::Element &el) override;
			/*! \brief Dumps the object to an XML element. Unsafe. */
			virtual xml::Element serialize(xml::Element &parent) const override;
			
			SVector points; /*!< Internal points storage */
			mutable SRect bbox; /*!< Temporary bounding box */

		CRN_DECLARE_CLASS_CONSTRUCTOR(Stroke)
		CRN_SERIALIZATION_CONSTRUCTOR(Stroke)
	};
	namespace protocol
	{
		template<> struct IsSerializable<Stroke> : public std::true_type {};
		template<> struct IsClonable<Stroke> : public std::true_type {};
	}

	/*! \brief Size of a stroke */
	inline size_t Size(const Stroke &s) noexcept { return s.GetNbPoints(); }
}
CRN_ADD_RANGED_FOR_TO_POINTERS(crn::Stroke)
CRN_ADD_RANGED_FOR_TO_CONST_POINTERS(crn::Stroke)

#endif
