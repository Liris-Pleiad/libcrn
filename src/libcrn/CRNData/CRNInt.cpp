/* Copyright 2009-2016 INSA Lyon, CoReNum, ENS-Lyon
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
 * file: CRNInt.cpp
 * \author Yann LEYDIER
 */

#include <CRNData/CRNInt.h>
#include <CRNException.h>
#include <CRNData/CRNDataFactory.h>
#include <CRNUtils/CRNXml.h>
#include <CRNi18n.h>

using namespace crn;

/*****************************************************************************/
/*!
 * Unsafe load
 *
 * \throws	ExceptionInvalidArgument	not an Int
 *
 * \param[in]	el	the element to load
 */
void Int::Deserialize(xml::Element &el)
{
	if (el.GetValue() != "Int")
	{
		throw ExceptionInvalidArgument(StringUTF8("void Int::Deserialize(xml::Element &el): ") + 
				_("Wrong XML element."));
	}
	val = el.GetAttribute<int>("value");
}

/*****************************************************************************/
/*!
 * Unsafe save
 *
 * \param[in]	parent	the parent element to which we will add the new element
 * \return The newly created element
 */
xml::Element Int::Serialize(xml::Element &parent) const
{
	xml::Element el(parent.PushBackElement("Int"));
	el.SetAttribute("value", val);
	return el;
}

CRN_BEGIN_CLASS_CONSTRUCTOR(Int)
	CRN_DATA_FACTORY_REGISTER(U"Int", Int)
CRN_END_CLASS_CONSTRUCTOR(Int)

