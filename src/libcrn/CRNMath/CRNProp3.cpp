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
 * file: CRNProp3.cpp
 * \author Yann LEYDIER
 */

#include <CRNMath/CRNProp3.h>
#include <CRNException.h>
#include <CRNData/CRNDataFactory.h>
#include <CRNUtils/CRNXml.h>
#include <CRNi18n.h>

#ifndef TRUE
#	define TRUE ~0
#endif

using namespace crn;

/*!
 * Constructor from integer value
 *
 * \param[in]	val	the value (TRUEval, FALSEval, anything else is UNKNOWNval)
 */
Prop3::Prop3(int val) noexcept
{
	if ((val == TRUEval) || (val == FALSEval))
		value = val;
	else if (val == int(TRUE))
		value = TRUEval;
	else
		value = UNKNOWNval;
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
		case TRUEval:
			return String(_("true"));
		case FALSEval:
			return String(_("false"));
		case UNKNOWNval:
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
	if ((prop.value == TRUEval) || (value == TRUEval))
		return Prop3(TRUEval);
	if ((prop.value == FALSEval) && (value == FALSEval))
		return Prop3(FALSEval);
	return Prop3(UNKNOWNval);
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
	if ((prop.value == FALSEval) || (value == FALSEval))
		return Prop3(FALSEval);
	if ((prop.value == TRUEval) && (value == TRUEval))
		return Prop3(TRUEval);
	return Prop3(UNKNOWNval);
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
	if (value == TRUEval)
		return Prop3(FALSEval);
	if (value == FALSEval)
		return Prop3(TRUEval);
	return Prop3(UNKNOWNval);
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
	if ((prop.value == TRUEval) || (value == TRUEval))
		value = TRUEval;
	else if ((prop.value == FALSEval) && (value == FALSEval))
		value = FALSEval;
	else
		value = UNKNOWNval;
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
	if ((prop.value == FALSEval) || (value == FALSEval))
		value = FALSEval;
	else if ((prop.value == TRUEval) && (value == TRUEval))
		value = TRUEval;
	else
		value = UNKNOWNval;
	return *this;
}

/*! \brief Is true? */
bool Prop3::IsTrue() const noexcept { return value == TRUEval; }
/*! \brief Is false? */
bool Prop3::IsFalse() const noexcept { return value == FALSEval; }
/*! \brief Is unknown? */
bool Prop3::IsUnknown() const noexcept { return value == UNKNOWNval; }

/*! 
 * Initialize the object from an XML element. Unsafe.
 *
 * \throws	ExceptionInvalidArgument	not a Prop3
 * \throws	ExceptionNotFound	attribute not found
 * \throws	ExceptionDomain	wrong attribute
 * \param[in]	el	the element to read
 */
void Prop3::Deserialize(xml::Element &el)
{
	if (el.GetName() != "Prop3")
	{
		throw ExceptionInvalidArgument(StringUTF8("bool Prop3::Deserialize(xml::Element &el): ") + 
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
xml::Element Prop3::Serialize(xml::Element &parent) const
{
	xml::Element el(parent.PushBackElement("Prop3"));
	el.SetAttribute("value", value);
	return el;
}

CRN_BEGIN_CLASS_CONSTRUCTOR(Prop3)
	CRN_DATA_FACTORY_REGISTER(U"Prop3", Prop3)
	Cloner::Register<Prop3>();
CRN_END_CLASS_CONSTRUCTOR(Prop3)

