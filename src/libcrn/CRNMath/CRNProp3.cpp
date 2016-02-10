/* Copyright 2006-2014 Yann LEYDIER, CoReNum, INSA-Lyon
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
 * file: CRNProp3.cpp
 * \author Yann LEYDIER
 */

#include <CRNMath/CRNProp3.h>
#include <CRNException.h>
#include <CRNData/CRNDataFactory.h>
#include <CRNUtils/CRNXml.h>
#include <CRNi18n.h>

using namespace crn;

const int Prop3::UNKNOWN = 2;
const Prop3 Prop3::True(TRUE);
const Prop3 Prop3::False(FALSE);
const Prop3 Prop3::Unknown(Prop3::UNKNOWN);

/*!
 * Constructor from integer value
 *
 * \param[in]	val	the value (TRUE, FALSE, anything else is UNKNOWN)
 */
Prop3::Prop3(int val) noexcept
{
	if ((val == TRUE) || (val == FALSE))
		value = val;
	else
		value = Prop3::UNKNOWN;
}

/*****************************************************************************/
/*!
 * Dump value to a string
 *
 * \return	the string
 */
String Prop3::ToString() const
{
	switch (value)
	{
		case TRUE:
			return String(_("true"));
		case FALSE:
			return String(_("false"));
		case Prop3::UNKNOWN:
		default:
			return String(_("unknown"));
	}	
}

/*!
 * Logical OR operator
 *
 * \param[in]	prop	the other value
 *
 * \return	the intersection
 */
Prop3 Prop3::operator|(const Prop3 &prop) const noexcept
{
	if ((prop.value == TRUE) || (value == TRUE))
		return Prop3(TRUE);
	if ((prop.value == FALSE) && (value == FALSE))
		return Prop3(FALSE);
	return Prop3(Prop3::UNKNOWN);
}

/*!
 * Logical AND operator
 *
 * \param[in]	prop	the other value
 *
 * \return	the union
 */
Prop3 Prop3::operator&(const Prop3 &prop) const noexcept
{
	if ((prop.value == FALSE) || (value == FALSE))
		return Prop3(FALSE);
	if ((prop.value == TRUE) && (value == TRUE))
		return Prop3(TRUE);
	return Prop3(Prop3::UNKNOWN);
}

/*!
 * Tests equality
 *
 * \param[in]	prop	the other value
 *
 * \return	true if the two values are equal, false else
 */
bool Prop3::operator==(const Prop3 &prop) const noexcept
{
	return prop.value == value;
}

/*!
 * Assignment operator
 *
 * \param[in]	prop	the new value
 *
 * \return	the newly reassign value
 */
Prop3& Prop3::operator=(const int &prop) noexcept
{
	return (*this = Prop3(prop));
}

/*!
 * Complementary operator
 * 
 * \return	returns the complementary of the value
 */
Prop3 Prop3::operator!() const noexcept
{
	if (value == TRUE)
		return Prop3(FALSE);
	if (value == FALSE)
		return Prop3(TRUE);
	return Prop3(Prop3::UNKNOWN);
}

/*!
 * Logical OR operator
 *
 * \param[in]	prop	the other value
 *
 * \return	the intersection
 */
Prop3& Prop3::operator|=(const Prop3 &prop) noexcept
{
	if ((prop.value == TRUE) || (value == TRUE))
		value = TRUE;
	else if ((prop.value == FALSE) && (value == FALSE))
		value = FALSE;
	else
		value = Prop3::UNKNOWN;
	return *this;
}

/*!
 * Logical AND operator
 *
 * \param[in]	prop	the other value
 *
 * \return	the union
 */
Prop3& Prop3::operator&=(const Prop3 &prop) noexcept
{
	if ((prop.value == FALSE) || (value == FALSE))
		value = FALSE;
	else if ((prop.value == TRUE) && (value == TRUE))
		value = TRUE;
	else
		value = Prop3::UNKNOWN;
	return *this;
}

/*! 
 * Initialize the object from an XML element. Unsafe.
 *
 * \throws	ExceptionInvalidArgument	not a Prop3
 * \throws	ExceptionNotFound	attribute not found
 * \throws	ExceptionDomain	wrong attribute
 * \param[in]	el	the element to read
 */
void Prop3::deserialize(xml::Element &el)
{
	if (el.GetName() != GetClassName().CStr())
	{
		throw ExceptionInvalidArgument(StringUTF8("bool Prop3::deserialize(xml::Element &el): ") + 
				_("Wrong XML element."));
	}
	value = el.GetAttribute<int>("value", false); // may throw
}

/*! 
 * Dump the object to an XML element. Unsafe.
 *
 * \param[in]	parent	the parent element
 *
 * \return	the newly created element
 */
xml::Element Prop3::serialize(xml::Element &parent) const
{
	xml::Element el(parent.PushBackElement(GetClassName().CStr()));
	el.SetAttribute("value", value);
	return el;
}

CRN_BEGIN_CLASS_CONSTRUCTOR(Prop3)
	CRN_DATA_FACTORY_REGISTER(U"Prop3", Prop3)
CRN_END_CLASS_CONSTRUCTOR(Prop3)

