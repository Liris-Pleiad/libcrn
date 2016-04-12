/* Copyright 2011-2014 CoReNum
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
 * file: CRNConfigElement.cpp
 * \author Yann LEYDIER
 */

#include <CRNUtils/CRNConfigElement.h>
#include <CRNi18n.h>

using namespace crn;

/*! Default constructor for serialization.
 * \attention	Do NOT use
 */
ConfigElement::ConfigElement():
	value(nullptr),
	minValue(nullptr),
	maxValue(nullptr)
{ }

/*! Creates a configuration element for an int value
 * \param[in]	nam	the translated name of the element
 * \param[in]	val	the initial value
 * \param[in]	desc	the description of the element
 */
ConfigElement::ConfigElement(const String& nam, int val, const String& desc):
	value(std::make_shared<Int>(val)),
	minValue(nullptr),
	maxValue(nullptr),
	name(nam),
	description(desc)
{ }

/*! Creates a configuration element for a double value
 * \param[in]	nam	the translated name of the element
 * \param[in]	val	the initial value
 * \param[in]	desc	the description of the element
 */
ConfigElement::ConfigElement(const String& nam, double val, const String& desc):
	value(std::make_shared<Real>(val)),
	minValue(nullptr),
	maxValue(nullptr),
	name(nam),
	description(desc)
{ }

/*! Creates a configuration element for a boolean value
 * \param[in]	nam	the translated name of the element
 * \param[in]	val	the initial value
 * \param[in]	desc	the description of the element
 */
ConfigElement::ConfigElement(const String& nam, bool val, const String& desc):
	value(std::make_shared<Prop3>(val)),
	minValue(nullptr),
	maxValue(nullptr),
	name(nam),
	description(desc)
{ }

/*! Creates a configuration element for a Prop3 value
 * \param[in]	nam	the translated name of the element
 * \param[in]	val	the initial value
 * \param[in]	desc	the description of the element
 */
ConfigElement::ConfigElement(const String& nam, const Prop3& val, const String& desc):
	value(Clone(val)),
	minValue(nullptr),
	maxValue(nullptr),
	name(nam),
	description(desc)
{ }

/*! Creates a configuration element for a String
 * \param[in]	nam	the translated name of the element
 * \param[in]	val	the initial value
 * \param[in]	desc	the description of the element
 */
ConfigElement::ConfigElement(const String& nam, const String& val, const String& desc):
	value(Clone(val)),
	minValue(nullptr),
	maxValue(nullptr),
	name(nam),
	description(desc)
{ }

/*! Creates a configuration element for a StringUTF8
 * \param[in]	nam	the translated name of the element
 * \param[in]	val	the initial value
 * \param[in]	desc	the description of the element
 */
ConfigElement::ConfigElement(const String& nam, const StringUTF8& val, const String& desc):
	value(Clone(val)),
	minValue(nullptr),
	maxValue(nullptr),
	name(nam),
	description(desc)
{ }

/*! Creates a configuration element for a Path
 * \param[in]	nam	the translated name of the element
 * \param[in]	val	the initial value
 * \param[in]	desc	the description of the element
 */
ConfigElement::ConfigElement(const String& nam, const Path& val, const String& desc):
	value(Clone(val)),
	minValue(nullptr),
	maxValue(nullptr),
	name(nam),
	description(desc)
{ }

/*! Gets the inner type of data in the element
 * \throws	ExceptionUninitialized	the element was not initialized
 * \throws	ExceptionInvalidArgument the element is of unknown type
 * \return	Int, Real, Prop3, String, StringUTF8 or Path
 */
String ConfigElement::GetType() const
{
	if (!value)
		throw	ExceptionUninitialized(_("The element was not initialized."));
	auto i = std::static_pointer_cast<Int>(value);
	if (i)
		return U"Int";
	auto r = std::static_pointer_cast<Real>(value);
	if (r)
		return U"Real";
	auto p = std::static_pointer_cast<Prop3>(value);
	if (p)
		return U"Prop3";
	auto s = std::static_pointer_cast<String>(value);
	if (s)
		return U"String";
	auto su = std::static_pointer_cast<StringUTF8>(value);
	if (su)
		return U"StringUTF8";
	auto pa = std::static_pointer_cast<Path>(value);
	if (pa)
		return U"Path";
	throw	ExceptionInvalidArgument(_("The element is of unknown type."));
}

