/* Copyright 2006-2016 Yann LEYDIER, CoReNum, INSA-Lyon, ENS-lyon
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
 * file: CRNReal.cpp
 * \author Yann LEYDIER
 */

#include <CRNData/CRNReal.h>
#include <math.h>
#include <CRNException.h>
#include <CRNMath/CRNProp3.h>
#include <CRNData/CRNDataFactory.h>
#include <CRNUtils/CRNXml.h>
#include <CRNMath/CRNMath.h>
#include <CRNi18n.h>

using namespace crn;

/*****************************************************************************/
/*!
 * Unsafe load
 *
 * \throws	ExceptionInvalidArgument	not a Real
 *
 * \param[in]	el	the element to load
 */
void Real::Deserialize(xml::Element &el)
{
	if (el.GetValue() != "Real")
	{
		throw ExceptionInvalidArgument(StringUTF8("void Real::Deserialize(xml::Element &el): ") + 
				_("Wrong XML element."));
	}
	val = el.GetAttribute<double>("value");
}

/*****************************************************************************/
/*!
 * Unsafe save
 *
 * \param[in]	parent	the parent element to which we will add the new element
 * \return The newly created element
 */
xml::Element Real::Serialize(xml::Element &parent) const
{
	xml::Element el(parent.PushBackElement("Real"));
	el.SetAttribute("value", val);
	return el;
}

CRN_BEGIN_CLASS_CONSTRUCTOR(Real)
	CRN_DATA_FACTORY_REGISTER(U"Real", Real)
	Cloner::Register<Real>();
	Ruler::Register<Real>();
CRN_END_CLASS_CONSTRUCTOR(Real)

