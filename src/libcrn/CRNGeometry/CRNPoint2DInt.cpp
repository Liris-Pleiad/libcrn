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
 * file: CRNPoint2DInt.cpp
 * \author Yann LEYDIER
 */

#include <CRNGeometry/CRNPoint2DInt.h>
#include <CRNData/CRNDataFactory.h>
#include <CRNException.h>
#include <CRNUtils/CRNXml.h>
#include <CRNi18n.h>

using namespace crn;

/*! 
 * Initializes the object from an XML element. Unsafe.
 *
 * \throws	ExceptionInvalidArgument	not a Point2DInt
 * \throws	ExceptionNotFound	attribute not found
 * \throws	ExceptionDomain	wrong attribute
 *
 * \param[in]	el	the XML element to read
 */
void Point2DInt::Deserialize(xml::Element &el)
{
	if (el.GetName() != "Point2DInt")
	{
		throw ExceptionInvalidArgument(StringUTF8("void Point2DInt::Deserialize(xml::Element &el): ") + 
				_("Wrong XML element."));
	}
	int x = el.GetAttribute<int>("x", false); // may throw
	int y = el.GetAttribute<int>("y", false); // may throw
	X = x;
	Y = y;
}

/*! 
 * Dumps the object to an XML element. Unsafe.
 *
 * \param[in]	parent	the element in which the new element will be stored
 * \return	the newly created element
 */
xml::Element Point2DInt::Serialize(xml::Element &parent) const
{
	xml::Element el(parent.PushBackElement("Point2DInt"));

	el.SetAttribute("x", X);
	el.SetAttribute("y", Y);

	return el;
}

/*! Moves the point towards a direction
 * \param[in]	dir	the direction of the move
 * \param[in]	step	the distance of the move
 */
void Point2DInt::Advance(const Direction &dir, int step)
{
	if (dir == Direction::LEFT)
		X -= step;
	else if (dir == Direction::RIGHT)
		X += step;
	else if (dir == Direction::TOP)
		Y -= step;
	else if (dir == Direction::BOTTOM)
		Y += step;
}

/*! Make a segment between two points
 * \param[in]	p_begin	the begining of the segment
 * \param[in]	p_end	the end of the segment
 * \return segment  vector of 2D points
 */
std::vector<Point2DInt> crn::MakeSegment(const Point2DInt &p_begin, const Point2DInt &p_end)
{
    std::vector<Point2DInt> segment;
    
    int x1 = p_begin.X;
    int y1 = p_begin.Y;
    int x2 = p_end.X;
    int y2 = p_end.Y;
    
    segment.push_back(Point2DInt(x1, y1));
    
    int lg_delta, sh_delta, cycle, lg_step, sh_step;
	lg_delta = (int)x2 - (int)x1;
	sh_delta = (int)y2 - (int)y1;
	if (lg_delta < 0)
	{
		lg_delta = -lg_delta;
		lg_step = -1;
	}
	else
		lg_step = 1;
    
	if (sh_delta < 0)
	{
		sh_delta = -sh_delta;
		sh_step = -1;
	}
	else
		sh_step = 1;
    
	if (sh_delta < lg_delta)
	{
		cycle = lg_delta >> 1;
		while (x1 != x2)
		{
			x1 += lg_step;
			cycle = cycle + sh_delta;
			if (cycle > lg_delta)
			{
				y1 += sh_step;
				cycle = cycle - lg_delta;
                
                segment.push_back(Point2DInt(x1, y1));
			}
		}
	}
	else
	{
		cycle = sh_delta >> 1;
		int tmp = lg_delta;
		lg_delta = sh_delta;
		sh_delta = tmp;
		tmp = lg_step;
		lg_step = sh_step;
		sh_step = tmp;
		while (y1 != y2)
		{
			y1 += lg_step;
			cycle = cycle + sh_delta;
			if (cycle > lg_delta)
			{
				x1 += sh_step;
				cycle = cycle - lg_delta;
                
                segment.push_back(Point2DInt(x1, y1));
			}
		}
	}
    
    segment.push_back(Point2DInt(x2, y2));
    
    return segment;
}

CRN_BEGIN_CLASS_CONSTRUCTOR(Point2DInt)
	CRN_DATA_FACTORY_REGISTER(U"Point2DInt", Point2DInt)
CRN_END_CLASS_CONSTRUCTOR(Point2DInt)

