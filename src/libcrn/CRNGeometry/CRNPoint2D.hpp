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
 * file: CRNPoint2D.hpp
 * \author Yann LEYDIER
 */

#include <CRNGeometry/CRNPoint2D.h>

namespace crn
{
	/*! 
	 * Converts object to string
	 * \return	a string representing the couple
	 */
	template<typename T> String Point2D<T>::ToString() const
	{
		return String(U'(') + X + String(U" × ") + Y + String(U')');
	}

	/*! 
	 * Distance between two points.
	 *
	 * \param[in]	obj	the other object
	 * \return	the distance between the two points
	 */
	template<typename T> double Point2D<T>::distance(const Object &obj) const
	{
		const Point2D<T> &p = static_cast<const Point2D<T>&>(obj);
		return sqrt(Sqr(double(X - p.X)) + Sqr(double(Y + p.Y)));
	}

	/*! 
	 * Unsafe method to add a 2D vector.
	 *
	 * \param[in]	v	the point to add
	 */
	template<typename T> void Point2D<T>::add(const Object &v)
	{
		const Point2D<T> &p = static_cast<const Point2D<T>&>(v);
		X += p.X;
		Y += p.Y;
	}

	/*! 
	 * Unsafe method to test the equality of two 2D points.
	 *
	 * \param[in]	v	the point to compare with
	 * \return	true if coordinates are equal, false else
	 */
	template<typename T> bool Point2D<T>::equals(const Object &v) const
	{
		const Point2D<T> &p = static_cast<const Point2D<T>&>(v);
		return (X == p.X) && (Y == p.Y);
	}

	/*! 
	 * Unsafe method to subtract a 2D vector.
	 *
	 * \param[in]	v	the point to subtract
	 */
	template<typename T> void Point2D<T>::sub(const Object &v)
	{
		const Point2D<T> &p = static_cast<const Point2D<T>&>(v);
		X -= p.X;
		Y -= p.Y;
	}

	/*! 
	 * Multiply by a real
	 *
	 * \param[in]	m	the multiplication factor
	 */
	template<typename T> void Point2D<T>::Mult(double m)
	{
		X = (T)(double(X) * m);
		Y = (T)(double(Y) * m);
	}

	/*! 
	 * Unsafe method to compute a sum of 2D vectors.
	 *
	 * \param[in]	plist	a list of pairs of point and coefficient
	 * \return	a new point
	 */
	template<typename T> UObject Point2D<T>::sum(const std::vector<std::pair<const Object*, double> > &plist) const
	{
		double ax = 0, ay = 0;
		for (unsigned int tmp = 0; tmp < plist.size(); tmp++)
		{
			Point2D<T> *p = (Point2D<T>*)plist[tmp].first;
			ax += double(p->X) * plist[tmp].second;
			ay += double(p->Y) * plist[tmp].second;
		}
		std::unique_ptr<Point2D<T>> p = CloneAs<Point2D<T>>();
		p->X = (T)ax;
		p->Y = (T)ay;
		return std::forward<std::unique_ptr<Point2D<T>>>(p);
	}

	/*! 
	 * Unsafe method to compute a mean of 2D vectors.
	 *
	 * \param[in]	plist	a list of pairs of point and coefficient
	 * \return	a new point
	 */
	template<typename T> UObject Point2D<T>::mean(const std::vector<std::pair<const Object*, double> > &plist) const
	{
		double tot = 0, ax = 0, ay = 0;
		for (unsigned int tmp = 0; tmp < plist.size(); tmp++)
		{
			tot += plist[tmp].second;
			Point2D<T> *p = (Point2D<T>*)plist[tmp].first;
			ax += double(p->X) * plist[tmp].second;
			ay += double(p->Y) * plist[tmp].second;
		}
		std::unique_ptr<Point2D<T>> p = CloneAs<Point2D<T>>();
		p->X = (T)(ax / tot);
		p->Y = (T)(ay / tot);
		return std::forward<std::unique_ptr<Point2D<T>>>(p);
	}

}
