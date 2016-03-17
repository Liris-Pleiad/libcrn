/* Copyright 2006-2016 Yann LEYDIER, Asma OUJI, CoReNum, INSA-Lyon, Université Paris Descartes, ENS-Lyon
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
 * file: CRNRect.h
 * \author Yann LEYDIER, Jean DUONG, Asma OUJI
 */

#ifndef CRNRECT_HEADER
#define CRNRECT_HEADER

#include <CRNObject.h>
#include <CRNGeometry/CRNPoint2DInt.h>
#include <CRNStatistics/CRNStatisticSample.h>
#include <iterator>
#include <CRNData/CRNForeach.h>
#include <CRNException.h>
#include <set>

namespace crn
{
	/****************************************************************************/
	/*! \brief A rectangle class
	 *
	 * class CRNRect
	 * Convenience rectangle class. Uses signed int.
	 *
	 * \author 	Yann LEYDIER
	 * \date		August 2006
	 * \version 0.1
	 * \ingroup geo
	 */
	class Rect: public Object
	{
		public:
			/*! \brief Dummy constructor. DO NOT USE. */
			Rect() noexcept:bx(0),by(0),ex(0),ey(0),w(0),h(0), valid(false) {}

			/*! \brief Rectangle constructor.
			 * 
			 * Rectangle constructor.
			 * 
			 * \param[in] 	begX leftmost coordinate
			 * \param[in] 	begY topmost coordinate
			 * \param[in] 	endX rightmost coordinate
			 * \param[in] 	endY bottommost coordinate
			 */
			inline Rect(int begX, int begY, int endX, int endY) noexcept:bx(begX),by(begY),ex(endX),ey(endY)
				{ w = ex - bx + 1; h = ey - by + 1; valid = (w >= 0) && (h >= 0); }

			/*! \brief Rectangle constructor.
			 * 
			 * Point-rectangle constructor.
			 * 
			 * \param[in] 	x abscissa coordinate
			 * \param[in] 	y ordinate coordinate
			 */
			Rect(int x, int y) noexcept:bx(x),by(y),ex(x),ey(y),w(1),h(1),valid(true) { }

			/*! \brief Rectangle constructor.
			 * 
			 * Point-rectangle constructor.
			 * 
			 * \param[in] 	p	a point
			 */
			Rect(const Point2DInt &p) noexcept:bx(p.X),by(p.Y),ex(p.X),ey(p.Y),w(1),h(1),valid(true) { }

			Rect(const Rect&) = default;
			Rect(Rect&&) = default;
			virtual ~Rect() override {}

			/*! \brief Clones the rectangle */
			virtual UObject Clone() const override { return std::make_unique<Rect>(bx, by, ex, ey); }

			Rect& operator=(const Rect&) = default;
			Rect& operator=(Rect&&) = default;

			/*! \brief Checks if two rectangles are identical */
			bool operator==(const Rect &r) const noexcept;
			/*! \brief Checks if two rectangles are different */
			bool operator!=(const Rect &r) const noexcept;

			/*! \brief Returns whether the rect is valid */
			inline bool IsValid() const noexcept { return valid; }

			/*! \brief Returns the leftmost coordinate
			 * \throws	ExceptionUninitialized	the rectangle is not properly initialized
			 */
			inline int GetLeft() const { if (!valid) throw ExceptionUninitialized("The rectangle isn't initialized."); return bx; }
			/*! \brief Returns the rightmost coordinate
			 * \throws	ExceptionUninitialized	the rectangle is not properly initialized
			 */
			inline int GetRight() const { if (!valid) throw ExceptionUninitialized("The rectangle isn't initialized."); return ex; }
			/*! \brief Returns the topmost coordinate
			 * \throws	ExceptionUninitialized	the rectangle is not properly initialized
			 */
			inline int GetTop() const { if (!valid) throw ExceptionUninitialized("The rectangle isn't initialized."); return by; }
			/*! \brief Returns the bottommost coordinate
			 * \throws	ExceptionUninitialized	the rectangle is not properly initialized
			 */
			inline int GetBottom() const { if (!valid) throw ExceptionUninitialized("The rectangle isn't initialized."); return ey; }
			/*! \brief Returns the width of the rectangle
			 * \throws	ExceptionUninitialized	the rectangle is not properly initialized
			 */
			inline int GetWidth() const { if (!valid) throw ExceptionUninitialized("The rectangle isn't initialized."); return w; }
			/*! \brief Returns the height of the rectangle
			 * \throws	ExceptionUninitialized	the rectangle is not properly initialized
			 */
			inline int GetHeight() const { if (!valid) throw ExceptionUninitialized("The rectangle isn't initialized."); return h; }
			/*! \brief Returns the top-left coordinates
			 * \throws	ExceptionUninitialized	the rectangle is not properly initialized
			 */
			inline Point2DInt GetTopLeft() const { if (!valid) throw ExceptionUninitialized("The rectangle isn't initialized."); return Point2DInt(bx, by); }
			/*! \brief Returns the bottom-right coordinates
			 * \throws	ExceptionUninitialized	the rectangle is not properly initialized
			 */
			inline Point2DInt GetBottomRight() const { if (!valid) throw ExceptionUninitialized("The rectangle isn't initialized."); return Point2DInt(ex, ey); }
			/*! \brief Returns the center coordinates
			 * \throws	ExceptionUninitialized	the rectangle is not properly initialized
			 */
			inline Point2DInt GetCenter() const { if (!valid) throw ExceptionUninitialized("The rectangle isn't initialized."); return Point2DInt((bx + ex) / 2, (by + ey) / 2); }
			/*! \brief Returns the horizontal center coordinate
			 * \throws	ExceptionUninitialized	the rectangle is not properly initialized
			 */
			inline int GetCenterX() const { if (!valid) throw ExceptionUninitialized("The rectangle isn't initialized."); return (bx + ex) / 2; }
			/*! \brief Returns the vertical center coordinate
			 * \throws	ExceptionUninitialized	the rectangle is not properly initialized
			 */
			inline int GetCenterY() const { if (!valid) throw ExceptionUninitialized("The rectangle isn't initialized."); return (by + ey) / 2; }

			/*! \brief Returns the area of the rectangle */
			unsigned int GetArea() const noexcept { return valid ? (w * h) : 0; }
			/*! \brief Returns the perimeter of the rectangle */
			inline unsigned int GetPerimeter() const noexcept
			{ 
				if (! valid)
					return 0;
				return (w*h == 1) ? 1 : (w + h) * 2 - 4; 
			}

			/*! \brief Changes the leftmost coordinate */
			inline int SetLeft(int begX) noexcept
			{
				bx = begX; 
				if(!valid) 
				{
					ex = bx;
					by = ey = 0;
					h = 1;
					valid = true;
				}
				w = ex - bx + 1; 
				return bx; 
			}
			/*! \brief Changes the rightmost coordinate */
			inline int SetRight(int endX) noexcept
			{
				ex = endX; 
				if (!valid)
				{
					bx = ex;
					by = ey = 0;
					h = 1;
					valid = true;
				}
				w = ex - bx + 1; 
				return ex; 
			}
			/*! \brief Changes the topmost coordinate */
			inline int SetTop(int begY) noexcept
			{
				by = begY;
				if (!valid)
				{
					ey = by;
					ex = bx = 0;
					w = 1;
					valid = true;
				}
				h = ey - by + 1; 
				return by;
			}
			/*! \brief Changes the bottommost coordinate */
			inline int SetBottom(int endY) noexcept
			{
				ey = endY; 
				if (!valid)
				{
					by = ey;
					ex = bx = 0;
					w = 1;
					valid = true;
				}
				h = ey - by + 1; 
				return ey; 
			}
			/*! \brief Changes the width of the rectangle
			 * \throws	ExceptionUninitialized	the rectangle is not properly initialized
			 */
			inline int SetWidth(int wid) 
			{
				if (!valid) 
					throw ExceptionUninitialized("The rectangle isn't initialized."); 
				w = wid; 
				ex = bx + w - 1;
				return w;
			}
			/*! \brief Changes the height of the rectangle
			 * \throws	ExceptionUninitialized	the rectangle is not properly initialized
			 */
			inline int SetHeight(int hei)
			{
				if (!valid) 
					throw ExceptionUninitialized("The rectangle isn't initialized."); 
				h = hei;
				ey = by + h - 1;
				return h;
			}
			
			/*! \brief Compute overlap in a given orientation */
			int Overlap(const Rect &r, Orientation orientation) const;
						
			/*! \brief Computes the intersection of two rectangles */
			Rect operator&(const Rect &r) const;
			/*! \brief Computes the union of two rectangles */
			Rect operator|(const Rect &r) const;
			/*! \brief Keeps only the intersecting part with another rectangle */
			void operator&=(const Rect &r);
			/*! \brief Merges with another rectangle */
			void operator|=(const Rect &r);

			/*! \brief Checks if the rectangle contains a point */
			bool Contains(int x, int y) const noexcept;
			/*! \brief Checks if the rectangle contains another rectangle */
			bool Contains(const Rect &rct) const;
        
			/*! \brief Scales the rectangle */
			const Rect& operator*=(double s);
			/*! \brief Creates a scaled rectangle */
			Rect operator*(double s) const;
			/*! \brief Translates the rectangle */
			void Translate(int x, int y);

			/*! \brief Get the smallest rectangle embedding a collection of rectangles */
			/*!
			 * Get the smallest rectangle embedding a collection of rectangles
			 *
			 * \param[in]	it_begin	The iterator pointing to the begining of the collection
			 * \param[in]	it_end	The iterator pointing to the end of the collection
			 * \return	The enbedding rectangle
			 */			
			template <class ITER> static Rect SmallestRectEmbedding(ITER it_begin, ITER it_end)
			{
				Rect first_rct = *it_begin;
				int l = first_rct.GetLeft();
				int r = first_rct.GetRight();
				int t = first_rct.GetTop();
				int b = first_rct.GetBottom();
				ITER it = it_begin + 1;
				while (it != it_end)
				{	
					Rect rb = *it;
					l = std::min(l, rb.GetLeft());
					r = std::max(r, rb.GetRight());
					t = std::min(t, rb.GetTop());
					b = std::max(b, rb.GetBottom());
					++it;
				}
				return Rect(l, t, r, b);
			}
			
			/*! \brief Get median width value for a collection of rectangles */
			/*!
			 * Get median width value for a collection of rectangles
			 * 
			 * \param[in]	it_begin	The iterator pointing to the begining of the collection
			 * \param[in]	it_end	The iterator pointing to the end of the collection
			 * \return	The median width value as a double number
			 */
			template <class ITER> static int MedianWidth(ITER it_begin, ITER it_end)			
			{
				auto w = std::vector<int>();
				for (auto it = it_begin; it != it_end; ++it)
					w.push_back(it->GetWidth());
				return MedianValue(w);
			}

			/*! \brief Get median height value for a collection of rectangles */
			/*!
			 * Get median height value for a collection of rectangles
			 * 
			 * \param[in]	it_begin	The iterator pointing to the begining of the collection
			 * \param[in]	it_end	The iterator pointing to the end of the collection
			 * \return	The median height value as a double number
			 */
			template <class ITER> static int MedianHeight(ITER it_begin, ITER it_end)			
			{
				auto h = std::vector<int>();
				for (auto it = it_begin; it != it_end; ++it)
					h.push_back(it->GetHeight());
				return MedianValue(h);
			}

			/*! \brief Get average width value for a collection of rectangles */
			/*!
			 * Get median width value for a collection of rectangles
			 * 
			 * \param[in]	it_begin	The iterator pointing to the begining of the collection
			 * \param[in]	it_end	The iterator pointing to the end of the collection
			 * \return	The average width value as a double number
			 */
			template <class ITER> static double MeanWidth(ITER it_begin, ITER it_end)			
			{
				double cumul = 0.0;
				double counter = 0.0;
				ITER it = it_begin;
				while (it != it_end)
				{
					cumul += ((*it).GetWidth());
					counter += 1.0;
					++it;
				}
				return cumul / std::max(1.0, counter);
			}

			/*! \brief Get median height value for a collection of rectangles */
			/*!
			 * Get median height value for a collection of rectangles
			 * 
			 * \param[in]	it_begin	The iterator pointing to the begining of the collection
			 * \param[in]	it_end	The iterator pointing to the end of the collection
			 * \return	The median height value as a double number
			 */
			template <class ITER> static double MeanHeight(ITER it_begin, ITER it_end)			
			{
				double cumul = 0.0;
				double counter = 0.0;
				ITER it = it_begin;
				while (it != it_end)
				{
					cumul += ((*it).GetHeight());
					counter += 1.0;
					++it;
				}
				return cumul / std::max(1.0, counter);
			}

			/*! \brief Get rectangles closest to border in a direction from a collection of rectangles */
			/*!
			 * Get rectangles closest to border in a given direction from a collection of rectangles
			 * 
			 * \param[in]	it_begin	The iterator pointing to the begining of the collection
			 * \param[in]	it_end	The iterator pointing to the end of the collection
			 * \param[in]	drt	The reference direction
			 * \return	A vector of minimal rectangles for direction
			 */			
			template <class ITER> static std::vector<Rect> FindClosestsToBorder(ITER it_begin, ITER it_end, Direction drt)			
			{
				std::vector<Rect> closest;
				
				ITER it_ref = it_begin;
				
				if (drt == Direction::LEFT)
				{
					while (it_ref != it_end)
					{
						bool is_extremal = true;
						int border_ref = it_ref->GetLeft();
					
						ITER it_cmp = it_begin;
					
						while ((it_cmp != it_end) && (is_extremal))
						{
							if (it_cmp != it_ref)
								if (it_ref->Overlap(*it_cmp, Orientation::VERTICAL))
									if (border_ref > it_cmp->GetLeft())
										is_extremal = false;
						
							++it_cmp;
						}
					
						if (is_extremal)
							closest.push_back(*it_ref);
					
						++it_ref;
					}
				}
				else if (drt == Direction::RIGHT)
				{
					while (it_ref != it_end)
					{
						bool is_extremal = true;
						int border_ref = it_ref->GetRight();
					
						ITER it_cmp = it_begin;
					
						while ((it_cmp != it_end) && (is_extremal))
						{
							if (it_cmp != it_ref)
								if (it_ref->Overlap(*it_cmp, Orientation::VERTICAL))
									if (border_ref < it_cmp->GetRight())
										is_extremal = false;
						
							++it_cmp;
						}
					
						if (is_extremal)
							closest.push_back(*it_ref);
					
						++it_ref;
					}
				}
				else if (drt == Direction::TOP)
				{
					while (it_ref != it_end)
					{
						bool is_extremal = true;
						int border_ref = it_ref->GetTop();
					
						ITER it_cmp = it_begin;
					
						while ((it_cmp != it_end) && (is_extremal))
						{
							if (it_cmp != it_ref)
								if (it_ref->Overlap(*it_cmp, Orientation::HORIZONTAL))
									if (border_ref > it_cmp->GetTop())
										is_extremal = false;
						
							++it_cmp;
						}
					
						if (is_extremal)
							closest.push_back(*it_ref);
					
						++it_ref;
					}
				}
				else if (drt == Direction::BOTTOM)
				{
					while (it_ref != it_end)
					{
						bool is_extremal = true;
						int border_ref = it_ref->GetBottom();
					
						ITER it_cmp = it_begin;
					
						while ((it_cmp != it_end) && (is_extremal))
						{
							if (it_cmp != it_ref)
								if (it_ref->Overlap(*it_cmp, Orientation::HORIZONTAL))
									if (border_ref < it_cmp->GetBottom())
										is_extremal = false;
						
							++it_cmp;
						}
					
						if (is_extremal)
							closest.push_back(*it_ref);
					
						++it_ref;
					}
				}

				return closest;
			}			
			
			/*! \brief Get the rectangles included in this */
			/*!
			 * Get the rectangles included in this
			 * 
			 * \param[in]	it_begin	The iterator pointing to the begining of the collection
			 * \param[in]	it_end	The iterator pointing to the end of the collection
			 * \return	A vector of rectangles included
			 */
			template <class ITER> std::vector<Rect> FindIncluded(ITER it_begin, ITER it_end)
			{
				std::vector<Rect> included;
				const auto r_left = GetLeft();
				const auto r_right = GetRight();
				const auto r_top = GetTop();
				const auto r_bottom = GetBottom();
				ITER it = it_begin;
				while (it != it_end)
				{	
					Rect r_i = *it;
					if((r_i.GetTop() >= r_top) && (r_i.GetBottom() <= r_bottom) && 
					   (r_i.GetLeft() >= r_left) && (r_i.GetRight() <= r_right))
					{
						included.push_back(r_i);
					}
					++it;
				}
				return included;
			}
			
			/*! \brief Get the rectangles intersecting significantly this from a collection */
			/*!
			 * Get the rectangles intersecting significantly this from a collection
			 * 
			 * \param[in]	it_begin	The iterator pointing to the begining of the collection
			 * \param[in]	it_end	The iterator pointing to the end of the collection
			 * \param[in]	ratio	The intersection significance threshold
			 * \return	A vector of rectangles intersecting
			 */
			template <class ITER> std::vector<Rect> FindIntersecting(ITER it_begin, ITER it_end, double ratio = 0)
			{
				ratio = std::max(ratio, 0.0);
				ratio = std::min(ratio, 1.0);
				std::vector<Rect> touching;
				ITER it = it_begin;
				while (it != it_end)
				{	
					Rect rb = *it;
					if (rb.IsValid())
					{
						Rect rct_inter = (*this)&rb;
						if((rct_inter).IsValid())
						{
							if (rct_inter.GetArea() > ratio * rb.GetArea())
							{
								touching.push_back(rb);
							}
						}
					}
					++it;
				}
				return touching;
			}
		
			/*! \brief Dumps to a string */
			virtual String ToString() const override;

			/*! \brief Functor to sort rectangles regarding directions
			 *
			 * Functor to sort rectangles considering directions
			 */
			class Sorter: public std::binary_function<const Rect&, const Rect&, bool>
			{
				public:
					/*! \brief Constructor */
					Sorter(Direction sort_direction);
					/*! \brief Comparison function */
					/*! 
					 * Comparison function "smaller than" regarding a given direction
					 * 
					 * \param[in]	r1	A rectangle, first parameter for comparison function
					 * \param[in]	r2	A rectangle, second parameter for comparison function
					 * \param[in]	true if r1 inferior to r2 for given direction, false else
					 */
					bool operator()(const Rect &r1, const Rect &r2) const;
				private:
					Direction direction; /*!< Direction for comparison */
			};

			/*! \brief Functor to sort rectangles regarding orthogonal directions
			 *
			 * Functor to sort rectangles considering orthogonal directions
			 */
			class OrthogonalSorter: public std::binary_function<const Rect&, const Rect&, bool>
			{
				public:
					/*! \brief Constructor */
					OrthogonalSorter(Direction sort_direction);
					/*! \brief Comparison function */
					/*! 
					 * Comparison function "smaller than" regarding a given reference direction and it's orthogonal complementary:
					 * A rectangle is smaller than another if it is inferior considering the reference direction and their projections in complementary direction overlap
					 * 
					 * \param[in]	r1	A rectangle, first parameter for comparison function
					 * \param[in]	r2	A rectangle, second parameter for comparison function
					 * \param[in]	true if r1 inferior to r2 for given direction and projections in complementary direction overlap, false else
					 */
					bool operator()(const Rect &r1, const Rect &r2) const;
				private:
					Direction direction; /*!< Direction for comparison */
			};

			/*! \brief Functor to sort rectangles by inclusion
			 *
			 * Functor to sort rectangles by inclusion
			 */
			 class InclusionSorter: public std::binary_function<const Rect&, const Rect&, bool>
			{
				public:
					/*! \brief Constructor */
					InclusionSorter(){}
					/*! \brief Comparison function */
					/*! 
					 * Comparison function "smaller than" regarding the inclusion relation
					 * 
					 * \param[in]	r1	A rectangle, first parameter for comparison function
					 * \param[in]	r2	A rectangle, second parameter for comparison function
					 * \param[in]	true if r1 included in r2, false else
					 */
					bool operator()(const Rect &r1, const Rect &r2) const;
			};

			/*! \brief Functor to sort rectangles regarding horizontal stretching values
			 *
			 * Functor to sort rectangles regarding horizontal stretching values
			 */
			class HorizontalStretchingSorter: public std::binary_function<const Rect&, const Rect&, bool>
			{
				public:
					/*! \brief Constructor */
					HorizontalStretchingSorter(){}
					/*! \brief Comparison function */
					/*! 
					 * Comparison function "smaller than"
					 * 
					 * \param[in]	r1	A rectangle, first parameter for comparison function
					 * \param[in]	r2	A rectangle, second parameter for comparison function
					 * \param[in]	true if r1 inferior to r2, false else
					 */
					bool operator()(const Rect &r1, const Rect &r2) const;
			};

			/*! \brief Functor to sort rectangles regarding surfaces
			 *
			 * Functor to sort rectangles considering areas
			 */
			class AreaSorter: public std::binary_function<const Rect&, const Rect&, bool>
			{
				public:
					/*! \brief Constructor
					 * \param[in]	biggertosmaller	true to sort from bigger to smaller, false to sort from smaller to bigger
					 */
					inline AreaSorter(bool biggertosmaller = false):big2low(biggertosmaller) {}
					/*! \brief Comparison function		
					 * 
					 * Comparison functor
					 * \param[in]	r1	the first rectangle to compare
					 * \param[in]	r2	the second rectangle to compare
					 * \return	true if r1 is closest to the reference border, false else
					 */
					inline bool operator()(const Rect &r1, const Rect &r2) const
					{	if (big2low)
							return r1.GetArea() > r2.GetArea();
						else
							return r1.GetArea() < r2.GetArea();
					}
				private:
					bool big2low;
			};

			/*! \brief Functor to sort rectangles regarding surfaces
			 *
			 * Functor to sort rectangles considering heights
			 */
			class HeightSorter: public std::binary_function<const Rect&, const Rect&, bool>
			{
				public:
					/*! \brief Constructor
					 * \param[in]	tallertosmaller	true to sort from taller to smaller, false to sort from smaller to taller
					 */
					inline HeightSorter(bool tallertosmaller = false):tall2small(tallertosmaller) {}
					/*! \brief Comparison function		
					 * 
					 * Comparison functor
					 * \param[in]	r1	the first rectangle to compare
					 * \param[in]	r2	the second rectangle to compare
					 * \return	true if r1 is closest to the reference border, false else
					 */
					inline bool operator()(const Rect &r1, const Rect &r2) const
					{	if (tall2small)
							return r1.GetHeight() > r2.GetHeight();
						else
							return r1.GetHeight() < r2.GetHeight();
					}
				private:
					bool tall2small;
			};


			/*! \brief Functor to sort Rects in sets and maps
			 *
			 * Functor to sort Rects in sets and maps
			 */
			struct BitwiseCompare: public std::binary_function<const Rect&, const Rect&, bool>
			{
				/*! \brief Comparison function */
				inline bool operator()(const Rect &r1, const Rect &r2) const
				{
					if (r1.GetLeft() < r2.GetLeft())
						return true;
					else if (r1.GetLeft() == r2.GetLeft())
					{
						if (r1.GetTop() < r2.GetTop())
							return true;
						else if (r1.GetTop() == r2.GetTop())
						{
							if ((r1.GetRight() < r2.GetRight()))
								return true;
							else if ((r1.GetRight() == r2.GetRight()))
							{
								if ((r1.GetBottom() < r2.GetBottom()))
									return true;
							}
						}
					}
					return false;
				}
			};
			/*! \brief A set of Rects */
			using Set = std::set<Rect, Rect::BitwiseCompare>;
			/*! \brief Creates an empty set of Rects */
			static Set EmptySet() { return Set(BitwiseCompare()); }

			/*! \brief iterator for a Rect
			 *
			 * The iterator sweeps the rectangle until invalid.
			 */
			class iterator: public std::iterator<std::input_iterator_tag, const crn::Point2DInt>
			{
				public:
					iterator() noexcept:valid(false) {}
					iterator(const Rect &r) noexcept:pos(r.GetLeft(), r.GetTop()),minx(r.GetLeft()),maxx(r.GetRight()),maxy(r.GetBottom()),valid(true) {}
					iterator(const iterator &) = default;
					iterator(iterator &&) = default;
					virtual ~iterator() {}
					iterator& operator=(const iterator &) = default;
					iterator& operator=(iterator &&) = default;
					bool operator==(const iterator &other) const noexcept;
					bool operator!=(const iterator &other) const noexcept { return !(other == *this); }
					virtual const iterator& operator++() noexcept;
					iterator operator++(int nouse) noexcept;
					reference operator*() const noexcept { return pos; }
					pointer operator->() const noexcept { return &pos; }
					bool IsValid() const noexcept { return valid; }
				protected:
					crn::Point2DInt pos;
					int minx, maxx, maxy;
					bool valid;
			};
		/*! \brief Returns an iterator to the first point of the rectangle */
		inline iterator begin() const { return iterator(*this); }
		/*! \brief Returns an iterator after the last point of the rectangle */
		inline iterator end() const { return iterator(); } 
		/*! \brief Returns an iterator to the first point of the rectangle */
		inline iterator cbegin() const { return iterator(*this); } 
		/*! \brief Returns an iterator after the last point of the rectangle */
		inline iterator cend() const { return iterator(); } 

			/*! \brief Spiral iterator for a Rect
			 *
			 * The iterator sweeps the rectangle from center to borders in a rectangular spiral until invalid.
			 */
		class spiral_iterator: public std::iterator<std::input_iterator_tag, const crn::Point2DInt>
		{
			public:
				spiral_iterator() {}
				spiral_iterator(const Rect &r);
				spiral_iterator(const spiral_iterator &) = default;
				spiral_iterator(spiral_iterator &&) = default;
				~spiral_iterator() {}
				spiral_iterator& operator=(const spiral_iterator &) = default;
				spiral_iterator& operator=(spiral_iterator &&) = default;
				bool operator==(const spiral_iterator &other) const;
				bool operator!=(const spiral_iterator &other) const { return !(other == *this); }
				const spiral_iterator& operator++();
				spiral_iterator operator++(int nouse);
				reference operator*() const { return pos; }
				pointer operator->() const { return &pos; }
				bool IsValid() const { return valid; }

			private:
				void update();
				int rectl = 0, rectt = 0, rectr = 0, rectb = 0;
				Point2DInt pos;
				Point2DInt ref;
				int border = 0;
				int framel = 0, framer = 0, framet = 0, frameb = 0;
				Direction dir;
				int offset = 0;
				int limit = 0;
				int pass = 0;
				Point2DInt pdir;
				bool preproc = false;
				bool valid = false;
		};
		/*! \brief Returns an spiral iterator to the first point of the rectangle */
		inline spiral_iterator SBegin() const { return spiral_iterator(*this); } 
		/*! \brief Returns an spiral iterator after the last point of the rectangle */
		inline spiral_iterator SEnd() const { return spiral_iterator(); } 

		private:
			/*! \brief Initializes the object from an XML element. Unsafe. */
			virtual void deserialize(xml::Element &el) override;
			/*! \brief Dumps the object to an XML element. Unsafe. */
			virtual xml::Element serialize(xml::Element &parent) const override;

			int bx, by, ex, ey; /*!< the coordinates */
			int w, h; /*!< the width and height */
			bool valid; /*!< whether the rectangle is valid */

		CRN_DECLARE_CLASS_CONSTRUCTOR(Rect)
		CRN_SERIALIZATION_CONSTRUCTOR(Rect)
	};
	namespace protocol
	{
		template<> struct IsSerializable<Rect> : public std::true_type {};
		template<> struct IsClonable<Rect> : public std::true_type {};
	}

	CRN_ALIAS_SMART_PTR(Rect)

}

CRN_ADD_RANGED_FOR_TO_POINTERS(crn::Rect)
CRN_ADD_RANGED_FOR_TO_CONST_POINTERS(crn::Rect)

#endif

