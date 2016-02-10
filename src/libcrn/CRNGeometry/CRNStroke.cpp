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
 * file: CRNStroke.cpp
 * \author Yann LEYDIER
 */

#include <CRNGeometry/CRNStroke.h>
#include <CRNException.h>
#include <limits>
#include <math.h>
#include <CRNData/CRNDataFactory.h>
#include <CRNUtils/CRNXml.h>
#include <CRNi18n.h>

using namespace crn;

/*!
 * Default constructor
 */
Stroke::Stroke():
	points(std::make_shared<Vector>()),
	bbox(nullptr)
{
}

/*!
 * Destructor
 */
Stroke::~Stroke()
{
}

/*!
 * Dumps to a string
 * \return	a string containing the points' coordinates
 */
String Stroke::ToString() const
{
	return points->ToString();
}

/*!
 * Clone the stroke
 *
 * \return	a new stroke
 */
UObject Stroke::Clone() const
{
	UStroke s(std::make_unique<Stroke>());
	for (size_t tmp = 0; tmp < points->Size(); tmp++)
		s->points->PushBack((*points)[tmp]->Clone());
	return std::forward<UStroke>(s);
}

/*! 
 * Initializes the object from an XML element. Unsafe.
 *
 * \throws	ExceptionInvalidArgument	not a Stroke
 * \throws	ExceptionDomain	CDATA not found
 *
 * \param[in]	el	the XML element to read
 */
void Stroke::deserialize(xml::Element &el)
{
	if (el.GetName() != GetClassName().CStr())
	{
		throw ExceptionInvalidArgument(StringUTF8("bool Stroke::deserialize(xml::Element &el): ") + 
				_("Wrong XML element."));
	}
	points->Clear();
	bbox = nullptr;
	xml::Node n(el.GetFirstChild());
	if (!n)
		return; // no content
	xml::Text t(n.AsText()); // may throw
	const std::vector<String> pts = String(t.GetValue()).Split(U" \n\r\t");
	for (size_t tmp = 0; tmp < pts.size() / 2; ++tmp)
		AddPoint(Point2DInt(pts[tmp + tmp].ToInt(), pts[tmp + tmp + 1].ToInt()));

}

/*! 
 * Dumps the object to an XML element. Unsafe.
 *
 * \param[in]	parent	the element in which the new element will be stored
 * \return	the newly created element
 */
xml::Element Stroke::serialize(xml::Element &parent) const
{
	xml::Element el(parent.PushBackElement(GetClassName().CStr()));
	StringUTF8 pts;
	for (Vector::const_iterator it = points->Begin(); it != points->End(); ++it)
	{
		SCPoint2DInt p(std::static_pointer_cast<const Point2DInt>(*it));
		pts += p->X;
		pts += " ";
		pts += p->Y;
		pts += " ";
	}
	el.PushBackText(pts);
	return el;
}

/*! 
 * Adds a point to the stroke 
 *
 *  \param[in]	p	the point to add
 */
void Stroke::AddPoint(const Point2DInt &p)
{
	points->PushBack(p.Clone());
	bbox = nullptr;
}

/*! 
 * Adds a point to the stroke keeping reference to the object. 
 * The object will be automatically destroyed when the stoke is destroyed.
 *
 *  \param[in]	p	the point to add
 */
void Stroke::AddAndKeepPoint(const SPoint2DInt &p)
{
	points->PushBack(p);
	bbox = nullptr;
}

/*****************************************************************************/
/*!
 * Adds a batch of points directly at the end of the stroke.
 * The first point of the merged stroke is propped on the last point of the current stroke.
 *
 * \throws	ExceptionInvalidArgument	invalid stroke
 * \param[in]	str	the stoke to append
 */
void Stroke::MergeAtEnd(const Stroke &str)
{
	if (&str == this)
		throw ExceptionInvalidArgument(_("Invalid stroke"));

	SPoint2DInt p = std::static_pointer_cast<Point2DInt>(points->Back());
	int offx = p->X;
	int offy = p->Y;
	p = std::static_pointer_cast<Point2DInt>(str.points->Front());
	offx -= p->X;
	offy -= p->Y;
	for (size_t tmp = 1; tmp < str.points->Size(); tmp++)
	{
		p = str.points->At(tmp)->CloneAs<Point2DInt>();
		p->X += offx;
		p->Y += offy;
		AddAndKeepPoint(p);
	}
 
	bbox = nullptr;
}

/*****************************************************************************/
/*!
 * Adds a batch of points at their original position and adds points to interpolate
 *
 * \throws	ExceptionInvalidArgument	null or invalid stroke
 * \throws	ExceptionDomain	null step
 * \param[in]	str	the stoke to append
 * \param[in]	step	the step of the interpolation
 */
void Stroke::MergeWithXInterpolation(const Stroke &str, unsigned int step)
{
	if (&str == this)
		throw ExceptionInvalidArgument(_("Invalid stroke"));

	if (step <= 0)
	{
		throw ExceptionDomain(StringUTF8("void Stroke::MergeWithXInterpolation("
					"const Stroke &str, double step): ") + 
				_("Null step."));
	}
	SPoint2DInt p = std::static_pointer_cast<Point2DInt>(points->Back());
	int bx = p->X;
	double by = p->Y;
	p = std::static_pointer_cast<Point2DInt>(str.points->Front());
	int ex = p->X;
	double ey = p->Y;

	if (Abs(ex - bx) > int(step))
	{
		double yoffset = (ey - by) / Abs(ex - bx);
		double y = by + yoffset;
		if (bx > ex)
		{ // right to left
			for (int x = bx - step; x > ex; x -= step)
			{
				AddPoint(Point2DInt(x, (int)y));
				y += yoffset;
			}
		}
		else
		{ // left to right
			for (int x = bx + step; x < ex; x += step)
			{
				AddPoint(Point2DInt(x, (int)y));
				y += yoffset;
			}
		}
	}

	for (size_t tmp = 0; tmp < str.points->Size(); tmp++)
	{
		p = std::static_pointer_cast<Point2DInt>(str.points->At(tmp));
		AddPoint(*p);
	}
	bbox = nullptr;
}

/*****************************************************************************/
/*!
 * Returns a reference to a point
 *
 * \throws	ExceptionDomain	index out of range
 *
 * \param[in]	index	the index of the point
 * \return	a pointer to the point or nullptr
 */
SPoint2DInt Stroke::GetPoint(size_t index)
{
	if (index >= points->Size())
		throw ExceptionDomain(StringUTF8("SPoint2DInt& Stroke::GetPoint(int index): ") + 
				_("index out of range."));
	return std::static_pointer_cast<Point2DInt>(points->At(index));
}

/*****************************************************************************/
/*!
 * Returns a reference to a point
 *
 * \throws	ExceptionDomain	index out of range
 *
 * \param[in]	index	the index of the point
 * \return	a pointer to the point or nullptr
 */
SPoint2DInt Stroke::GetPoint(size_t index) const
{
	if (index >= points->Size())
		throw ExceptionDomain(StringUTF8("SPoint2DInt& Stroke::GetPoint(int index) const: ") + 
				_("index out of range."));
	return std::static_pointer_cast<Point2DInt>(points->At(index));
}

/*****************************************************************************/
/*!
 * Returns the ordinate of the first point with abscissa = x
 *
 * \throws	ExceptionDomain	cannot reach abscissa
 *
 * \param[in]	x	the abscissa
 * \return	the ordinate or NaN if not found
 */
double Stroke::GetFirstYAtX(double x) const
{
	for (const_iterator it = Begin(); it != End(); ++it)
		if (it->X == x)
			return it->Y;
	throw ExceptionDomain(StringUTF8("Double Stroke::GetFirstYAtX(double x) const:") + 
			_("Cannot reach abscissa."));
}

/*****************************************************************************/
/*!
 * Returns the bounding box using integer values
 *
 * \return	a int rectangle
 */
Rect Stroke::GetBBox() const
{
	if (points->Size() > 0)
	{
		// is the bbox already computed?
		if (bbox)
			return *bbox;
		// else update bbox
		SPoint2DInt p = std::static_pointer_cast<Point2DInt>(points->Front());
		Rect r(p->X, p->Y, p->X, p->Y);
		for (size_t tmp = 1; tmp < points->Size(); tmp++)
		{
			p = std::static_pointer_cast<Point2DInt>(points->At(tmp));
			r |= Rect(p->X, p->Y, p->X, p->Y);
		}
		bbox = r.CloneAs<Rect>(); // store to prevent recomputing
		return r;
	}
	else
		return Rect();
}

/*****************************************************************************/
/*!
 * Creates a new stroke that is the intersection of the stroke with a rectangle
 *
 * \param[in]	rect	the bounding box of the resulting stroke
 * \return	a new stroke
 */
UStroke Stroke::MakeIntersection(const Rect &rect) const
{
	UStroke s = std::make_unique<Stroke>();
	for (const_iterator it = Begin(); it != End(); ++it)
	{
		if (rect.Contains((int)it->X, (int)it->Y))
		{
			s->AddPoint(**it);
		}
	}
	return std::forward<UStroke>(s);
}

/*****************************************************************************/
/*!
 * Creates a new stroke that is the first intersection of the stroke with a rectangle
 *
 * \param[in]	rect	the bounding box of the resulting stroke
 * \return	a new stroke
 */
UStroke Stroke::MakeFirstIntersection(const Rect &rect) const
{
	UStroke s = std::make_unique<Stroke>();
	bool filling = false;
	for (const_iterator it = Begin(); it != End(); ++it)
	{
		if (rect.Contains((int)it->X, (int)it->Y))
		{
			filling = true;
			s->AddPoint(**it);
		}
		else if (filling)
		{ // stop after the first intersection
			break;
		}
	}
	return std::forward<UStroke>(s);
}

CRN_BEGIN_CLASS_CONSTRUCTOR(Stroke)
	CRN_DATA_FACTORY_REGISTER(U"Stroke", Stroke)
CRN_END_CLASS_CONSTRUCTOR(Stroke)

