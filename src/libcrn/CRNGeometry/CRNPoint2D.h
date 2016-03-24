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
 * file: CRNPoint2D.h
 * \author Yann LEYDIER
 */

#ifndef CRNPOINT2D_HEADER
#define CRNPOINT2D_HEADER

#include <CRNObject.h>
#include <CRNString.h>
#include <CRNMath/CRNMath.h>
#include <type_traits>

/*! \defgroup geo Geometry
 * \ingroup math */

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
	template<typename T> class Point2D: public Object
	{
		public:
			/*! \brief The value type */
			using value_type = T;

			/*! \brief Default constructor (sets coordinates to 0) */
			Point2D() noexcept(noexcept(T{0})) : X(0),Y(0) {}
			/*! \brief Constructor from values */
			Point2D(T x, T y) noexcept(std::is_nothrow_copy_constructible<T>::value) : X(x),Y(y) {}
			Point2D(const Point2D &) = default;
			Point2D(Point2D &&) = default;
			/*! \brief Destructor */
			virtual ~Point2D() override = default;

			/*! \brief Converts object to string */
			String ToString() const { return String(U'(') + X + String(U" × ") + Y + String(U')'); }

			value_type X, Y; /*!< The coordinates */

			/*! \brief Converts to another type of Point */
			template<typename U> Point2D<U> Convert() const noexcept(noexcept(U{X})) { return Point2D<U>(U(X), U(Y)); }

			Point2D& operator=(const Point2D &) = default;
			Point2D& operator=(Point2D &&) = default;

			bool operator==(const Point2D &p) const noexcept { return (X == p.X) && (Y == p.Y); }
			/*! \brief Translation */
			Point2D<T>& operator+=(const Point2D<T> &p) noexcept { X += p.X; Y += p.Y; return *this; }
			/*! \brief Translation */
			Point2D<T>& operator-=(const Point2D<T> &p) noexcept { X -= p.X; Y -= p.Y; return *this; }

			/*! \brief Functor to compare points */
			struct Comparer
			{
				public:
					/*! \brief Constructor
					 * \param[in]	dir	the direction LEFT = left to right, BOTTOM = bottom to top, etc.
					 */
					Comparer(Direction dir): direction(dir) { }
					/*! \brief functor */
					bool operator()(const Point2D &p1, const Point2D &p2) const
					{
						if (direction == Direction::LEFT)
							return p1.X < p2.X;
						else if (direction == Direction::RIGHT)
							return p1.X > p2.X;
						else if (direction == Direction::TOP)
							return p1.Y < p2.Y;
						else
							return p1.Y > p2.Y;
					}
				private:
					Direction direction; /*!< direction of the sort */
			};
	};

	template<typename T> double Distance(const Point2D<T> &p1, const Point2D<T> &p2, DistanceType dt = DistanceType::EUCLIDEAN)
	{
		switch (dt)
		{
			case DistanceType::D4:
				return Abs(p1.X - p2.X) + Abs(p1.Y - p2.Y);
			case DistanceType::D8:
				return Max(Abs(p1.X - p2.X), Abs(p1.Y - p2.Y));
			case DistanceType::EUCLIDEAN:
				return sqrt(Sqr(p1.X - p2.X) + Sqr(p1.Y - p2.Y));
			default:
				throw ExceptionInvalidArgument("double Distance(const Point2D<T>&, const Point2D<T>&, DistanceType): invalid distance type.");
		}
	}
}

template<typename T> crn::Point2D<T> operator+(crn::Point2D<T> p1, const crn::Point2D<T> &p2) noexcept { return p1 += p2; }
template<typename T> crn::Point2D<T> operator-(crn::Point2D<T> p1, const crn::Point2D<T> &p2) noexcept { return p1 -= p2; }
template<typename T> crn::Point2D<double> operator*(double d, const crn::Point2D<T> &p) noexcept { return {p.X * d, p.Y * d}; }
template<typename T> crn::Point2D<double> operator*(const crn::Point2D<T> &p, double d) noexcept { return {p.X * d, p.Y * d}; }

#endif
