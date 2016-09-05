/* Copyright 2012-2016 CoReNum, ENS-Lyon
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
 * file: CRNLinearInterpolation.cpp
 * \author Yann LEYDIER
 */

#include <CRNMath/CRNLinearInterpolation.h>
#include <CRNData/CRNDataFactory.h>
#include <CRNi18n.h>

using namespace crn;

double LinearInterpolation::operator[](double x) const
{
	double slope, x0, y0;
	if (x < data.front().X)
	{
		x0 = data[0].X;
		y0 = data[0].Y;
		slope = (data[1].Y - y0) / (data[1].X - x0);
	}
	if (x > data.back().X)
	{
		x0 = data.back().X;
		y0 = data.back().Y;
		slope = (data[data.size() - 2].Y - y0) / (data[data.size() - 2].X - x0);
	}
	else
	{
		for (size_t tmp = 0; tmp < data.size() - 1; ++tmp)
		{
			if (data[tmp].X == x)
				return data[tmp].Y;
			else if ((data[tmp].X <= x) && (data[tmp + 1].X >= x))
			{
				x0 = data[tmp].X;
				y0 = data[tmp].Y;
				slope = (data[tmp + 1].Y - y0) / (data[tmp + 1].X - x0);
				break;
			}
		}
	}
	return y0 + slope * (x - x0);
}

/*! 
 * Initializes the object from an XML element. Unsafe.
 *
 * \throws	ExceptionInvalidArgument	not a Point2DDouble
 * \throws	ExceptionNotFound	attribute not found
 * \throws	ExceptionDomain	wrong attribute
 *
 * \param[in]	el	the XML element to read
 */
void LinearInterpolation::Deserialize(xml::Element &el)
{
	if (el.GetName() != "LinearInterpolation")
	{
		throw ExceptionInvalidArgument(StringUTF8("void LinearInterpolation::Deserialize(xml::Element &el): ") + 
				_("Wrong XML element."));
	}

	std::vector<crn::Point2DDouble> newdata;
	for (xml::Element sel = el.BeginElement(); sel != el.EndElement(); ++sel)
	{
		newdata.push_back(Point2DDouble());
		newdata.back().Deserialize(sel); // may throw
	}
	data.swap(newdata);
}

/*! 
 * Dumps the object to an XML element. Unsafe.
 *
 * \param[in]	parent	the element in which the new element will be stored
 * \return	the newly created element
 */
xml::Element LinearInterpolation::Serialize(xml::Element &parent) const
{
	xml::Element el(parent.PushBackElement("LinearInterpolation"));

	for (const Point2DDouble &p : data)
		p.Serialize(el);

	return el;
}

CRN_BEGIN_CLASS_CONSTRUCTOR(LinearInterpolation)
	CRN_DATA_FACTORY_REGISTER(U"LinearInterpolation", LinearInterpolation)
	Cloner::Register<LinearInterpolation>();
CRN_END_CLASS_CONSTRUCTOR(LinearInterpolation)


