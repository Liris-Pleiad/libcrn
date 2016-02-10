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
			virtual ~Point2D() override {}

			const String& GetClassName() const override { static const String cn(U"Point2D<T>"); return cn; }
			/*! \brief This is a Metric and VectorOverR object */
			virtual Protocol GetClassProtocols() const noexcept override { return crn::Protocol::Metric|crn::Protocol::VectorOverR; } 
			
			/*! \brief Converts object to string */
			virtual String ToString() const override;

			value_type X, Y; /*!< The coordinates */

			/*! \brief Converts to another type of Point */
			template<typename U> Point2D<U> Convert() const noexcept(noexcept(U{X})) { return Point2D<U>(U(X), U(Y)); }

			inline Point2D& operator=(const Point2D &) = default;
			inline Point2D& operator=(Point2D &&) = default;
			/*! \brief Translation */
			inline const Point2D<T>& operator+=(const Point2D<T> &p) { add(p); return *this; }
			/*! \brief Addition */
			inline Point2D<T> operator+(const Point2D<T> &p) const { Point2D<T> np(*this); np.add(p); return np; }
			/*! \brief Equality */
			inline bool operator==(const Point2D<T> &p) const { return equals(p); }
			/*! \brief Translation */
			inline const Point2D<T>& operator-=(const Point2D<T> &p) { sub(p); return *this; }
			/*! \brief Subtraction */
			inline Point2D<T> operator-(const Point2D<T> &p) const { Point2D<T> np(*this); np.sub(p); return np; }
			/*! \brief Homothetic transform */
			inline const Point2D<T>& operator*=(double d) { Mult(d); return *this; }
			/*! \brief Multiplication */
			inline Point2D<T> operator*(double d) { Point2D<T> np(*this); np.Mult(d); return np; }
			/*! \brief Homothetic transform */
			inline const Point2D<T>& operator/=(double d) { Mult(1/d); return *this; }
			/*! \brief Division */
			inline Point2D<T> operator/(double d) { Point2D<T> np(*this); np.Mult(1/d); return np; }

			/*! \brief Multiply by a real */
			virtual void Mult(double m) override;
				
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
		private:
			/*! \brief Distance between two points. */
			virtual double distance(const Object &obj) const override;
			/*! \brief Unsafe method to add a 2D vector. */
			virtual void add(const Object &v) override;
			/*! \brief Unsafe method to test the equality of two 2D points. */
			virtual bool equals(const Object &v) const override;
			/*! \brief Unsafe method to subtract a 2D vector. */
			virtual void sub(const Object &v) override;
			/*! \brief Unsafe method to compute a sum of 2D vectors. */
			virtual UObject sum(const std::vector<std::pair<const Object*, double> > &plist) const override;
			/*! \brief Unsafe Method to compute a mean of 2D vectors. */
			virtual UObject mean(const std::vector<std::pair<const Object*, double> > &plist) const override;
			
	};
}

#include <CRNGeometry/CRNPoint2D.hpp>

#endif
