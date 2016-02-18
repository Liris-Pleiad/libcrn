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
 * file: CRNDataFactory.cpp
 * \author Yann LEYDIER
 */

#include <CRNData/CRNDataFactory.h>
#include <CRNException.h>
#include <CRNString.h>
#include <CRNIO/CRNIO.h>
#include <CRNi18n.h>

using namespace crn;

DataFactory& DataFactory::getInstance()
{
	static DataFactory fac;
	return fac;
}

/*! 
 * Creates and initializes a CRNData from an XML element 
 *
 * \throws	ExceptionInvalidArgument	null XML element
 * \throws	ExceptionRuntime	impossible to read the XML element
 * \throws	ExceptionNotFound	unknown type
 *
 * \param[in]	el	the XML element to use for initialization
 * \return	a pointer to the newly created object, or nullptr if failed
 */
UObject DataFactory::CreateData(xml::Element &el)
{
	if (!el)
	{
		throw ExceptionInvalidArgument(StringUTF8("SObject DataFactory::CreateData(xml::Element &el): ") + 
				_("Null XML element."));
	}
	const StringUTF8 typ = el.GetValue();
	if (typ.IsEmpty())
	{
		throw ExceptionRuntime(StringUTF8("SObject DataFactory::CreateData(xml::Element &el): ") + _("Cannot read element."));
	}
	return getInstance().createObject(typ, el); // may throw
}

/*!
 * Registers a class in the factory
 *
 * \param[in]	name	the class name
 * \param[in]	cstr	the constructor for this class
 * \return	false if already registered, true else
 */
bool DataFactory::RegisterClass(const String &name, std::unique_ptr<DataFactoryElementBase> &&cstr)
{
	if (getInstance().data.find(name) == getInstance().data.end())
	{
		getInstance().data[name] = std::move(cstr);
		return true;
	}
	else
		return false;
}

/*!
 * Returns the list of registered classes
 *
 * \return	a string containing the registered class names
 */
String DataFactory::GetKnownTypes()
{
	String s;
	
	for (auto & elem : getInstance().data)
	{
		s += (elem).first + U"\n";
	}
	return s;
}

/*!
 * Default constructor
 */
DataFactory::DataFactory()
{
}

/*!
 * Destructor
 */
DataFactory::~DataFactory()
{
}

/*!
 * Creates a SObject from class name
 *
 * \throws	ExceptionNotFound	unknown type
 *
 * \param[in]	name	the name of the class
 * \return	a new object
 */
UObject DataFactory::createObject(const String &name, xml::Element &el)
{
	if (data.find(name) != data.end())
		return data[name]->Create(el);
	else
	{
		throw ExceptionNotFound(StringUTF8("SObject CRNDataFactory::createObject("
					"const String &name, xml::Element &el): " + StringUTF8(_("Unknown type: "))) + 
				name.CStr() + StringUTF8("."));
	}
}

