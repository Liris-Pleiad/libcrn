/* Copyright 2013-2016 INSA Lyon, ENS-Lyon
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
 * file: CRNPoint3D.h
 * \author Jean DUONG
 */

#ifndef CRNPOINT3D_HEADER
#define CRNPOINT3D_HEADER

#include <CRNObject.h>
#include <CRNString.h>

namespace crn
{
	/****************************************************************************/
	/*! \brief A 3D point class
	 *
	 * Convenience 3D point class.
	 *
	 * \author 	Jean DUONG
	 * \date	February 2013
	 * \version 0.1
	 * \ingroup geo
	 */
	template<typename T> class Point3D
	{
		public:
			/*! \brief The value type */
			using value_type = T;

			/*! \brief Default constructor (sets coordinates to 0) */
			Point3D():X(0),Y(0),Z(0) {}
			/*! \brief Constructor from values */
			Point3D(T x, T y, T z):X(x),Y(y),Z(z) {}
			Point3D(const Point3D &) = default;
			Point3D(Point3D &&) = default;
			/*! \brief Destructor */
			virtual ~Point3D() {}
									
			value_type X, Y, Z; /*!< The coordinates */

			Point3D& operator=(const Point3D &) = default;
			Point3D& operator=(Point3D &&) = default;
			/*! \brief Equality */
			bool operator==(const Point3D &p) const noexcept { return (X == p.X) && (Y == p.Y) && (Z == p.Z); }

			/*! \brief Functor to sort rectangles regarding directions */
			/*!
			 * Functor to sort rectangles considering directions
			 */
			class Sorter: public std::binary_function<const Point3D&, const Point3D&, bool>
			{
				public:
					/*! \brief Comparison function */
					/*! 
					 * Comparison function "smaller than"
					 * 
					 * \param[in]	p1	A 3D point, first parameter for comparision function
					 * \param[in]	p2	A 3D point, second parameter for comparision function
					 * \return	true if r1 inferior to r2, false else
					 */
					inline bool operator()(const Point3D &p1, const Point3D &p2) const
					{	
						if (p1.X < p2.X) return true;
						else if (p1.X > p2.X) return false;
						
						if (p1.Y < p2.Y) return true;
						else if (p1.Y > p2.Y) return false;
						
						if (p1.Z < p2.Z) return true;
						
						return false;
					}
			};
	};
}

#endif
