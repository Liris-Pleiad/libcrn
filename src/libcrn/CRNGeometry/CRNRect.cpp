/* Copyright 2006-2016 Yann LEYDIER, CoReNum, INSA-Lyon, ENS-Lyon
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
 * file: CRNRect.cpp
 * \author Yann LEYDIER
 */

#include <CRNIO/CRNIO.h>
#include <limits>

#include <CRNGeometry/CRNRect.h>
#include <CRNData/CRNDataFactory.h>
#include <CRNUtils/CRNXml.h>
#include <CRNi18n.h>

using namespace crn;

/*****************************************************************************/
/*!
 * Checks if two rectangles are identical
 *
 * \param[in]	r	The rectangle to check
 * \return	true if identical, false else
 */
bool Rect::operator==(const Rect &r) const noexcept
{
	if (valid && r.valid && (GetLeft() == r.GetLeft()) && (GetRight() == r.GetRight()) && 
			(GetTop() == r.GetTop()) && (GetBottom() == r.GetBottom()))
		return true;
	else if (!valid && !r.valid)
		return true;
	else
		return false;
}

/*****************************************************************************/
/*!
 * Checks if two rectangles are different
 *
 * \param[in]	r	The rectangle to check
 * \return	true if different, false else
 */
bool Rect::operator!=(const Rect &r) const noexcept
{
	if (!valid || !r.valid)
		return true;
	else if ((GetLeft() != r.GetLeft()) || (GetRight() != r.GetRight()) || 
			(GetTop() != r.GetTop()) || (GetBottom() != r.GetBottom()))
		return true;
	else
		return false;
}

/*****************************************************************************/
/*!
 * Computes the intersection of two rectangles
 *
 * \param[in]	r	The rectangle to intersect
 * \return	The intersecting rectangle
 */
Rect Rect::operator&(const Rect &r) const
{
	if (valid && r.valid)
	{
		int left, right, top, bottom;
		left = Max(GetLeft(), r.GetLeft());
		right = Min(GetRight(), r.GetRight());
		top = Max(GetTop(), r.GetTop());
		bottom = Min(GetBottom(), r.GetBottom());
		if ((left <= right) && (top <= bottom))
		{
			return Rect(left, top, right, bottom);
		}
	}
	return Rect();
}

/*****************************************************************************/
/*!
 * Computes overlap of with another rectangle in a given orientation
 *
 * \throws	ExceptionInvalidArgument	no orientation given
 * \param[in]	r	The other rectangle
 * \param[in]	orientation	the orientation in which the overlapping is considered
 * \return	The overlap length if result>0, or the distance between the rectangles if result<0
 */

int Rect::Overlap(const Rect &r, Orientation orientation) const
{
	if (!!(orientation & Orientation::HORIZONTAL))
	{
		return std::min(GetBottom(), r.GetBottom()) - std::max(GetTop(), r.GetTop());
	}
	else if (!!(orientation & Orientation::VERTICAL))
	{
		return std::min(GetRight(), r.GetRight()) - std::max(GetLeft(), r.GetLeft());
	}
	else
		throw ExceptionInvalidArgument(_("No orientation given."));
}



/*****************************************************************************/
/*!
 * Computes the union of two rectangles
 *
 * \param[in]	r	The rectangle to unite
 * \return	The union rectangle
 */
Rect Rect::operator|(const Rect &r) const
{
	if (valid && r.valid)
	{
		int left, right, top, bottom;
		left = Min(GetLeft(), r.GetLeft());
		right = Max(GetRight(), r.GetRight());
		top = Min(GetTop(), r.GetTop());
		bottom = Max(GetBottom(), r.GetBottom());
		return Rect(left, top, right, bottom);
	}
	if (valid)
		return Rect(bx, ex, by, ey);
	if (r.valid)
		return Rect(r.bx, r.ex, r.by, r.ey);
	return Rect();
}

/*!
 * Keeps only the intersecting part with another rectangle
 *
 * \param[in]	r	The intersecting rectangle
 */
void Rect::operator&=(const Rect &r)
{
	if (valid && r.valid)
	{
		int left, right, top, bottom;
		left = Max(GetLeft(), r.GetLeft());
		right = Min(GetRight(), r.GetRight());
		top = Max(GetTop(), r.GetTop());
		bottom = Min(GetBottom(), r.GetBottom());
		if (left > right || top > bottom)
		{
			valid = false;
		}
		else
		{
			SetLeft(left);
			SetRight(right);
			SetTop(top);
			SetBottom(bottom);
		}
	}
	else
		valid = false;
}

/*****************************************************************************/
/*!
 * Merges with another rectangle
 *
 * \param[in]	r	The rectangle to merge
 */
void Rect::operator|=(const Rect &r)
{
	if (valid && r.valid)
	{
		int left, right, top, bottom;
		left = Min(GetLeft(), r.GetLeft());
		right = Max(GetRight(), r.GetRight());
		top = Min(GetTop(), r.GetTop());
		bottom = Max(GetBottom(), r.GetBottom());
		SetLeft(left);
		SetRight(right);
		SetTop(top);
		SetBottom(bottom);
	}
	else if (!valid && r.valid)
	{
		SetLeft(r.bx);
		SetRight(r.ex);
		SetTop(r.by);
		SetBottom(r.ey);
	}
}

/*****************************************************************************/
/*!
 * Creates a string with the coordiantes of the rectangle
 * \return	the string
 */
String Rect::ToString() const
{
	if (valid)
	{
		String s(U"(");
		s += bx;
		s += U" x ";
		s += by;
		s += U"), (";
		s += ex;
		s += U" x ";
		s += ey;
		s += U") -> (";
		s += w;
		s += U" x ";
		s += h;
		s += U")";
		return s;
	}
	else
		return String(_("The rectangle isn't initialized."));
}

/*!
 * Comparison functor constructor for direction sorter
 *
 * \throws	ExceptionInvalidArgument	invalid direction
 *
 * \param[in]	sort_direction	the reference border of the plan for comparison. Example: is sort_direction is Direction::Top, the rectangles will be sorted from the topmost to the bottommost.
 */
Rect::Sorter::Sorter(Direction sort_direction)
{
	if ((sort_direction == Direction::LEFT) || (sort_direction == Direction::RIGHT) || 
			(sort_direction == Direction::TOP) || (sort_direction == Direction::BOTTOM))
	{
		direction = sort_direction;
	}
	else
	{
		throw ExceptionInvalidArgument(StringUTF8("Rect::Sorter::Sorter(int sort_direction): ") + 
				_("Wrong direction."));
	}
}

/*!
 * Comparison functor 
 *
 * \param[in]	r1	the first rectangle to compare
 * \param[in]	r2	the second rectangle to compare
 * \return	true if r1 is closest to the reference border, false else
 */
bool Rect::Sorter::operator()(const Rect &r1, const Rect &r2) const
{
	if (direction == Direction::RIGHT)
	{
		// right to left
		if (r1.GetRight() < r2.GetRight())
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else if (direction == Direction::LEFT)
	{
		// left to right
		if (r1.GetLeft() < r2.GetLeft())
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else if (direction == Direction::TOP)
	{
		// top to bottom
		if (r1.GetTop() < r2.GetTop())
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else if (direction == Direction::BOTTOM)
	{
		// bottom to top
		if (r1.GetBottom() < r2.GetBottom())
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	return false;
}


/*!
 * Comparison functor constructor for direction sorter
 *
 * \throws	ExceptionInvalidArgument	invalid direction
 *
 * \param[in]	sort_direction	the reference border of the plan for comparison. Example: is sort_direction is Direction::Top, the rectangles will be sorted from the topmost to the bottommost.
 */
Rect::OrthogonalSorter::OrthogonalSorter(Direction sort_direction)
{
	if ((sort_direction == Direction::LEFT) || (sort_direction == Direction::RIGHT) || 
			(sort_direction == Direction::TOP) || (sort_direction == Direction::BOTTOM))
	{
		direction = sort_direction;
	}
	else
	{
		throw ExceptionInvalidArgument(StringUTF8("Rect::Sorter::Sorter(int sort_direction): ") + 
				_("Wrong direction."));
	}
}

/*!
 * Comparison functor 
 *
 * \param[in]	r1	the first rectangle to compare
 * \param[in]	r2	the second rectangle to compare
 * 
 * \return	true if r1 is closer to the reference border than r2, false else
 */
bool Rect::OrthogonalSorter::operator()(const Rect &r1, const Rect &r2) const
{
	if (direction == Direction::RIGHT)
	{
		// right to left
		if ((r1.GetRight() > r2.GetRight()) && (r1.Overlap(r2, Orientation::HORIZONTAL) > 0))
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else if (direction == Direction::LEFT)
	{
		// left to right
		if ((r1.GetLeft() < r2.GetLeft()) && (r1.Overlap(r2, Orientation::HORIZONTAL) > 0))
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else if (direction == Direction::TOP)
	{
		// top to bottom
		if ((r1.GetTop() < r2.GetTop()) && (r1.Overlap(r2, Orientation::VERTICAL) > 0))
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else if (direction == Direction::BOTTOM)
	{
		// bottom to top
		if ((r1.GetBottom() > r2.GetBottom()) && (r1.Overlap(r2, Orientation::VERTICAL) > 0))
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	return false;
}

/*!
 * Comparison functor 
 *
 * \param[in]	r1	the first rectangle to compare
 * \param[in]	r2	the second rectangle to compare
 * 
 * \return	true if r1 is closest to the reference border, false else
 */
bool Rect::InclusionSorter::operator()(const Rect &r1, const Rect &r2) const
{
	int t = r1.GetTop();
	int b = r1.GetBottom();
	int l = r1.GetLeft();
	int r = r1.GetRight();
	
	return (r2.Contains(l, t) && r2.Contains(l, b) && r2.Contains(r, t) && r2.Contains(r, b));
}

/*!
 * Comparison functor 
 *
 * \param[in]	r1	the first rectangle to compare
 * \param[in]	r2	the second rectangle to compare
 * 
 * \return	true if r1 has stretching value smaller than r2, false else
 */
bool Rect::HorizontalStretchingSorter::operator()(const Rect &r1, const Rect &r2) const
{
	double s1 = (double)r1.GetWidth() / (double)r1.GetHeight();
	double s2 = (double)r2.GetWidth() / (double)r2.GetHeight();
	
	return (s1 < s2);
}

/*! Scales the rectangle
 *
 * \throws	ExceptionUninitialized	uninitialized rectangle
 * \throws	ExceptionInvalidArgument	negative scale
 *
 * \param[in]	s	the scale
 * 
 * \return	a new rectangle
 */
const Rect& Rect::operator*=(double s)
{
	if (!valid)
		throw ExceptionUninitialized(_("The rectangle isn't initialized."));
	if (s <= 0.0)
		throw ExceptionInvalidArgument(_("negative scale."));

	Rect r((int)(GetLeft() * s), (int)(GetTop() * s));
	r.SetWidth(Max((int)(GetWidth() * s), 1));
	r.SetHeight(Max((int)(GetHeight() * s), 1));
	*this = r;
	return *this;
}

/*****************************************************************************/
/*!
 * Creates a scaled rectangle
 *
 * \throws	ExceptionUninitialized	uninitialized rectangle
 * \throws	ExceptionInvalidArgument	negative scale
 *
 * \param[in]	s	the scale
 * \return	a new rectangle
 */
Rect Rect::operator*(double s) const
{
	if (!valid)
		throw ExceptionUninitialized(_("The rectangle isn't initialized."));
	if (s <= 0.0)
		throw ExceptionInvalidArgument(_("negative scale."));

	Rect r((int)(GetLeft() * s), (int)(GetTop() * s));
	r.SetWidth(Max((int)(GetWidth() * s), 1));
	r.SetHeight(Max((int)(GetHeight() * s), 1));
	return r;
}

bool Rect::iterator::operator==(const Rect::iterator &other) const noexcept
{
	if (!valid && !other.valid)
		return true;
	if ((valid && other.valid) && (pos == other.pos) && 
			(minx == other.minx) && (maxx == other.maxx) && (maxy == other.maxy))
			return true;
	return false;
}

const Rect::iterator& Rect::iterator::operator++() noexcept
{ 
	if (valid) 
	{ 
		pos.X += 1; 
		if (pos.X > maxx) 
		{ 
			pos.X = minx; 
			pos.Y += 1; 
			if (pos.Y > maxy) 
				valid = false; 
		} 
	} 
	return *this;
}

Rect::iterator Rect::iterator::operator++(int nouse) noexcept
{
	Rect::iterator temp = *this;
	++(*this);
	return temp;
}

/*****************************************************************************/
/*!
 * Checks if the rectangle contains a point
 *
 * \param[in]	x	the abscissa
 * \param[in]	y	the ordinate
 * \return	true if the point is inside the rectangle, false else
 */
bool Rect::Contains(int x, int y) const noexcept
{
	if (x < bx)
		return false;
	if (x > ex)
		return false;
	if (y < by)
		return false;
	if (y > ey)
		return false;
	return valid;
}

/*!
 * Checks if the rectangle contains another rectangle
 *
 * \param[in]	rct	the possibly contained rectangle
 * \return	true if the candidate is inside the rectangle, false else
 */
bool Rect::Contains(const Rect &rct) const
{
    int t = rct.GetTop();
    int b = rct.GetBottom();
    int l = rct.GetLeft();
    int r = rct.GetRight();
    
	return (Contains(l, t) && Contains(r, t) && Contains(l, b) && Contains(r, b));
}

/*! 
 * Initializes the object from an XML element. Unsafe.
 *
 * \throws	ExceptionInvalidArgument	not a Rect
 *
 * \param[in]	el	the XML element to read
 */
void Rect::deserialize(xml::Element &el)
{
	if (el.GetName() != GetClassName().CStr())
	{
		throw ExceptionInvalidArgument(StringUTF8("bool Rect::deserialize(xml::Element &el): ") + 
				_("Wrong XML element."));
	}
	bx = el.GetAttribute<int>("bx");
	ex = el.GetAttribute<int>("ex");
	by = el.GetAttribute<int>("by");
	ey = el.GetAttribute<int>("ey");
	try
	{
		valid = el.GetAttribute<bool>("v", false);
	}
	catch (ExceptionNotFound&)
	{
		valid = true;
	}
	w = ex - bx + 1;
	h = ey - by + 1;
}

/*! 
 * Dumps the object to an XML element. Unsafe.
 *
 * \param[in]	parent	the element in which the new element will be stored
 * \return	the newly created element
 */
xml::Element Rect::serialize(xml::Element &parent) const
{
	xml::Element el(parent.PushBackElement(GetClassName().CStr()));
	el.SetAttribute("bx", bx);
	el.SetAttribute("ex", ex);
	el.SetAttribute("by", by);
	el.SetAttribute("ey", ey);
	el.SetAttribute("v", valid ? 1 : 0);
	return el;
}

/*!
 * Translates the rectangle
 *
 * \throws	ExceptionUninitialized	the rectangle is not properly initialized
 *
 * \param[in]	x	the x translation term
 * \param[in]	y	the y translation term
 */
void Rect::Translate(int x, int y)
{
	if (!valid)
		throw ExceptionUninitialized(_("The rectangle isn't initialized."));
	bx += x;
	ex += x;
	by += y;
	ey += y;
}

Rect::spiral_iterator::spiral_iterator(const Rect &r):
	rectl(r.GetLeft()),
	rectt(r.GetTop()),
	rectr(r.GetRight()),
	rectb(r.GetBottom()),
	ref(r.GetCenterX(), r.GetCenterY()),
	dir(((r.GetWidth() >= r.GetHeight()) ? Direction::RIGHT : Direction::TOP)),
	pass(0),
	preproc(false),
	valid(true)
{
	pos = ref;
	border = Min(ref.X - r.GetLeft(), ref.Y - r.GetTop());
	framel = rectl + border;
	framet = rectl + border;
	framer = rectr - border;
	frameb = rectb - border;
	if (((framel == framer) || (framet == frameb)) && (r.GetWidth() != r.GetHeight()))
	{
		preproc = true;
		if (framel == framer)
		{
			pdir = Point2DInt(0, 1);
			offset = pos.Y - r.GetTop();
			limit = pos.X - r.GetLeft() - 1;
			if (r.GetHeight() % 2)
				pass = 1;
		}
		else
		{
			pdir = Point2DInt(1, 0);
			offset = pos.X - r.GetLeft();
			limit = pos.Y - r.GetTop() - 1;
			if (r.GetWidth() % 2)
				pass = 1;
		}
	}
}

bool Rect::spiral_iterator::operator==(const spiral_iterator &other) const
{
	if (!valid && !other.valid)
		return true;
	if ((valid && other.valid) && (pos == other.pos) && 
			(rectl == other.rectl) && (rectt == other.rectt) && 
			(rectr == other.rectr) && (rectb == other.rectb))
			return true;
	return false;
}

const Rect::spiral_iterator& Rect::spiral_iterator::operator++()
{
	if (!valid)
		return *this;
	if (preproc)
	{
		pos = ref;
		pass ^= 1;
		if (pass == 0)
		{
			offset -= 1;
			if (offset <= limit)
			{
				preproc = false;
				update();
				return *this;
			}
			if (pdir.X)
				pos.X = rectl + offset;
			else
				pos.Y = rectt + offset;
		}
		else
		{
			if (pdir.X)
				pos.X = rectr - offset;
			else
				pos.Y = rectb - offset;
		}
	}
	else
	{
		if (dir == Direction::RIGHT)
		{
			if (pos.X + 1 > framer)
			{
				dir = Direction::BOTTOM;
			}
			else
				pos.X += 1;
		}
		if (dir == Direction::BOTTOM)
		{
			if (pos.Y + 1 > frameb)
			{
				dir = Direction::LEFT;
			}
			else
				pos.Y += 1;
		}
		if (dir == Direction::LEFT)
		{
			if (pos.X - 1 < framel)
			{
				dir = Direction::TOP;
			}
			else
				pos.X -= 1;
		}
		if (dir == Direction::TOP)
		{
			if (pos.Y - 1 < framet)
			{
				dir = Direction::RIGHT;
				update(); // ---------,--> equivalent to goto @"if (dir = RIGHT)"
				return ++*this;	// --/
			}
			else
				pos.Y -= 1;
		}
		update();
	} // no preproc
	return *this;
}

Rect::spiral_iterator Rect::spiral_iterator::operator++(int nouse)
{
	spiral_iterator temp = *this;
	++(*this);
	return temp;
}

void Rect::spiral_iterator::update()
{
	if (pos == ref)
	{
		border -= 1;
		if (border < 0)
		{
			valid = false;
			return;
		}

		framel = rectl + border;
		framet = rectt + border;
		framer = rectr - border;
		frameb = rectb - border;
		if (dir == Direction::RIGHT)
			pos.Y -= 1;
		else if (dir == Direction::BOTTOM)
			pos.X += 1;
		else if (dir == Direction::LEFT)
			pos.Y += 1;
		else if (dir == Direction::TOP)
			pos.X -= 1;
		ref = pos;
	}
}

CRN_BEGIN_CLASS_CONSTRUCTOR(Rect)
	CRN_DATA_FACTORY_REGISTER(U"Rect", Rect)
CRN_END_CLASS_CONSTRUCTOR(Rect)

