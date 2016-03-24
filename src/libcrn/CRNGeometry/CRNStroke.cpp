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
Stroke::Stroke() = default;
/*!
 * Destructor
 */
Stroke::~Stroke() = default;

Stroke::Stroke(const Stroke &other):
	points(other.points)
{
	if (other.bbox)
		bbox = CloneAs<Rect>(*other.bbox);
}

Stroke& Stroke::operator=(const Stroke &other)
{
	points = other.points;
	if (other.bbox)
		bbox = CloneAs<Rect>(*other.bbox);
	return *this;
}

/*! 
 * Initializes the object from an XML element. Unsafe.
 *
 * \throws	ExceptionInvalidArgument	not a Stroke
 * \throws	ExceptionDomain	CDATA not found
 *
 * \param[in]	el	the XML element to read
 */
void Stroke::Deserialize(xml::Element &el)
{
	if (el.GetName() != "Stroke")
	{
		throw ExceptionInvalidArgument(StringUTF8("void Stroke::Deserialize(xml::Element &el): ") + 
				_("Wrong XML element."));
	}
	points.clear();
	bbox = nullptr;
	xml::Node n(el.GetFirstChild());
	if (!n)
		return; // no content
	xml::Text t(n.AsText()); // may throw
	const auto pts = String(t.GetValue()).Split(U" \n\r\t");
	for (size_t tmp = 0; tmp < pts.size() / 2; ++tmp)
		AddPoint(Point2DInt(pts[tmp + tmp].ToInt(), pts[tmp + tmp + 1].ToInt()));
}

/*! 
 * Dumps the object to an XML element. Unsafe.
 *
 * \param[in]	parent	the element in which the new element will be stored
 * \return	the newly created element
 */
xml::Element Stroke::Serialize(xml::Element &parent) const
{
	xml::Element el(parent.PushBackElement("Stroke"));
	StringUTF8 pts;
	for (const auto &p : points)
	{
		pts += p.X;
		pts += " ";
		pts += p.Y;
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
	points.push_back(p);
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

	auto p = points.back();
	int offx = p.X;
	int offy = p.Y;
	p = str.points.front();
	offx -= p.X;
	offy -= p.Y;
	for (size_t tmp = 1; tmp < str.points.size(); tmp++)
	{
		p = str.points[tmp];
		p.X += offx;
		p.Y += offy;
		AddPoint(p);
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
	auto p = points.back();
	int bx = p.X;
	double by = p.Y;
	p = str.points.front();
	int ex = p.X;
	double ey = p.Y;

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

	for (const auto &p : str.points)
	{
		AddPoint(p);
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
Point2DInt& Stroke::GetPoint(size_t index)
{
	if (index >= points.size())
		throw ExceptionDomain(StringUTF8("Point2DInt& Stroke::GetPoint(int index): ") + 
				_("index out of range."));
	return points[index];
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
const Point2DInt& Stroke::GetPoint(size_t index) const
{
	if (index >= points.size())
		throw ExceptionDomain(StringUTF8("SPoint2DInt& Stroke::GetPoint(int index) const: ") + 
				_("index out of range."));
	return points[index];
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
	for (const auto &p : points)
		if (p.X == x)
			return p.Y;
	throw ExceptionDomain(StringUTF8("double Stroke::GetFirstYAtX(double x) const:") + 
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
	if (points.size() > 0)
	{
		// is the bbox already computed?
		if (bbox)
			return *bbox;
		// else update bbox
		auto fp = points.front();
		Rect r(fp.X, fp.Y, fp.X, fp.Y);
		for (size_t tmp = 1; tmp < points.size(); tmp++)
		{
			const auto &p = points[tmp];
			r |= Rect(p.X, p.Y, p.X, p.Y);
		}
		bbox = CloneAs<Rect>(r); // store to prevent recomputing
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
Stroke Stroke::MakeIntersection(const Rect &rect) const
{
	auto s = Stroke();
	for (const auto &p : points)
	{
		if (rect.Contains(p.X, p.Y))
		{
			s.AddPoint(p);
		}
	}
	return s;
}

/*****************************************************************************/
/*!
 * Creates a new stroke that is the first intersection of the stroke with a rectangle
 *
 * \param[in]	rect	the bounding box of the resulting stroke
 * \return	a new stroke
 */
Stroke Stroke::MakeFirstIntersection(const Rect &rect) const
{
	auto s = Stroke();
	auto filling = false;
	for (const auto &p : points)
	{
		if (rect.Contains(p.X, p.Y))
		{
			filling = true;
			s.AddPoint(p);
		}
		else if (filling)
		{ // stop after the first intersection
			break;
		}
	}
	return s;
}

CRN_BEGIN_CLASS_CONSTRUCTOR(Stroke)
	CRN_DATA_FACTORY_REGISTER(U"Stroke", Stroke)
	Cloner::Register<Stroke>();
CRN_END_CLASS_CONSTRUCTOR(Stroke)

