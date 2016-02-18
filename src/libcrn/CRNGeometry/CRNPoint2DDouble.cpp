/* Copyright 2008-2016 INSA Lyon, CoReNum
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
 * file: CRNPoint2DDouble.cpp
 * \author Yann LEYDIER
 */

#include <CRNGeometry/CRNPoint2DDouble.h>
#include <CRNData/CRNDataFactory.h>
#include <CRNException.h>
#include <CRNUtils/CRNXml.h>
#include <CRNi18n.h>

using namespace crn;

/*! 
 * Initializes the object from an XML element. Unsafe.
 *
 * \throws	ExceptionInvalidArgument	not a Point2DDouble
 * \throws	ExceptionNotFound	attribute not found
 * \throws	ExceptionDomain	wrong attribute
 *
 * \param[in]	el	the XML element to read
 */
void Point2DDouble::deserialize(xml::Element &el)
{
	if (el.GetName() != GetClassName().CStr())
	{
		throw ExceptionInvalidArgument(StringUTF8("bool Point2DDouble::deserialize(xml::Element &el): ") + 
				_("Wrong XML element."));
	}

	double x = el.GetAttribute<double>("x", false); // may throw
	double y = el.GetAttribute<double>("y", false); // may throw
	X = x;
	Y = y;
}

/*! 
 * Dumps the object to an XML element. Unsafe.
 *
 * \param[in]	parent	the element in which the new element will be stored
 * \return	the newly created element
 */
xml::Element Point2DDouble::serialize(xml::Element &parent) const
{
	xml::Element el(parent.PushBackElement(GetClassName().CStr()));

	el.SetAttribute("x", X);
	el.SetAttribute("y", Y);

	return el;
}

/*!
 * Perform plan rotation for a single point
 *
 * \param[in]	theta	the rotation angle (in radian)
 *
 * \return the new point
 */
Point2DDouble Point2DDouble::MakeRotation(Angle<Radian> theta) const
{
	double cos_theta = theta.Cos();
	double sin_theta = theta.Sin();
		
	return Point2DDouble(X * cos_theta - Y * sin_theta, X * sin_theta + Y * cos_theta);	
}

/*!
 * Perform plan rotation for many points
 *
 * \param[in]	P	a set of points to rotate
 * \param[in]	theta	the rotation angle (in radian)
 *
 * \return the new point
 */
std::vector<Point2DDouble> Point2DDouble::MakeRotation(std::vector<Point2DDouble> P, Angle<Radian> theta)
{
	double cos_theta = theta.Cos();
	double sin_theta = theta.Sin();	
	
	std::vector<Point2DDouble> Q;

	if (!P.empty())
	{
		
		for (auto pt : P)
		{
			
			double x = pt.X;
			double y = pt.Y;
			
			Q.push_back(Point2DDouble(x * cos_theta - y * sin_theta, x * sin_theta + y * cos_theta));
		}		
	}
	
	return Q;
}

CRN_BEGIN_CLASS_CONSTRUCTOR(Point2DDouble)
	CRN_DATA_FACTORY_REGISTER(U"Point2DDouble", Point2DDouble)
CRN_END_CLASS_CONSTRUCTOR(Point2DDouble)

