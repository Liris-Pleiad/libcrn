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
 * file: CRNMap.cpp
 * \author Yann LEYDIER
 */

#include <CRNData/CRNMap.h>
#include <CRNException.h>
#include <CRNData/CRNVector.h>
#include <CRNData/CRNDataFactory.h>
#include <CRNUtils/CRNXml.h>
#include <CRNIO/CRNIO.h>
#include <CRNi18n.h>
using namespace crn;

/*!
 * Default constructor
 * \param[in]	protos	the mandatory protocols for the contents
 */
Map::Map(Protocol protos):
	protocols(protos)
{
}

/*!
 * Destructor
 */
Map::~Map()
{
}

/*!
 * Returns the protocols supported by the container depending on its contents.
 * \return the protocols supported by the container.
 */
Protocol Map::GetClassProtocols() const noexcept
{
	return protocols & (crn::Protocol::Object|crn::Protocol::Clonable|crn::Protocol::Serializable);
}

/*!
 * Retrieves an object from key
 *
 * \param[in]	s	the key
 * \return	a pointer to the object or nullptr if key not found
 */
SObject Map::Get(const String &s)
{
	iterator it = Find(s);
	if (it == End())
		return nullptr;
	else
		return it->second;
}

/*!
 * Retrieves an object from key
 *
 * \param[in]	s	the key
 * \return	a pointer to the object or nullptr if key not found
 */
SCObject Map::Get(const String &s) const
{
	const_iterator it = Find(s);
	if (it == End())
		return nullptr;
	else
		return it->second;
}


/*!
 * Adds an object
 *
 * \throw	ExceptionProtocol	value does not conform to protocols
 *
 * \param[in]	key	the key of the object to add
 * \param[in]	value	the object to add
 */
void Map::Set(const String &key, SObject value)
{
	if (checkProtocols(*value.get()))
	{
		data[key] = value;
	}
	else
		throw ExceptionProtocol(_("Value object does not conform to the map's protocols."));
}

/*!
 * Remove an object from the container by key
 *
 * \throws	ExceptionNotFound	key not found
 *
 * \param[in]	key	the key
 */
void Map::Remove(const String &key)
{
	if (!data.erase(key))
		throw ExceptionNotFound(_("Key not found"));
}

/*!
 * Remove an object from the container by pointer
 *
 * \throws	ExceptionNotFound	object not found
 *
 * \param[in]	obj	a pointer to the object to remove
 */
void Map::Remove(const SObject &obj)
{
	for (iterator it = Begin(); it != End(); ++it)
		if (it->second == obj)
		{
			Remove(it->first);
			return;
		}
	throw ExceptionNotFound(_("Object not found."));
}

/*!
 * Removes an element (safe) 
 *
 * \throws	ExceptionDomain	invalid iterator
 *
 * \param[in]  it  an iterator to the element to remove
 */
void Map::Remove(iterator it)
{
	if (it == End())
		throw ExceptionDomain(_("Invalid iterator."));
	data.erase(it);
}

/*!
 * Removes a list of elements (safe)
 *
 * \throws	ExceptionDomain	invalid first iterator
 * \throws	ExceptionInvalidArgument	invalid range
 *
 * \param[in]	first	the first element to remove
 * \param[in]	end	the element after the last element to remove
 * \return	true if success, false else
 */
void Map::Remove(iterator first, iterator end)
{
	// cannot begin with End()
	if (first == End())
		throw ExceptionDomain(_("First iterator is end()."));
	// null range
	if (first == end)
		throw ExceptionInvalidArgument(_("First and end iterators are equal."));
	// if end is End()
	if (end == End())
	{
		data.erase(first, end);
	}
	// if end is not the End() of the map, check that it is really after first
	for (iterator tmp = first; tmp != End(); ++tmp)
	{
		if (tmp == end)
		{
			data.erase(first, end);
			return;
		}
	}
	throw ExceptionInvalidArgument(_("End iterator is before first."));
}

/*!
 * Dumps the contents to a string
 * \return	a string representing the contents of the container
 */
String Map::ToString() const
{
	String s = GetClassName();
	s += " { ";
	for (const_iterator it = Begin(); it != End(); ++it)
	{
		s += String(U"[") + it->first + U"]";
		s += it->second->ToString();
		const_iterator nit(it);
		++nit;
		if (nit != End())
			s += ", ";
	}
	s += " }";
	return s;
}

/*!
 * Clones the container and its contents if applicable. 
 * \throws	ExceptionProtocol	the content of the map is not clonable
 *
 * \return	the cloned container
 */
UObject Map::Clone() const
{
	if (!(protocols & crn::Protocol::Clonable))
		throw ExceptionProtocol(_("This map does not contain clonable objects."));

	UMap m = std::make_unique<Map>(protocols);
	m->SetName(GetName());
	for (const_iterator it = Begin(); it != End(); ++it)
		m->Set(it->first, it->second->Clone());
	return std::forward<UMap>(m);
}

/*****************************************************************************/
/*!
 * Unsafe load
 *
 * \throws	ExceptionProtocol	the content of the map is not serializable
 * \throws	ExceptionInvalidArgument	not a Map
 *
 * \param[in]	el	the element to load
 * \return true if success, false else
 */
void Map::deserialize(xml::Element &el)
{
	if (!(protocols & crn::Protocol::Serializable))
		throw ExceptionProtocol(_("This map does not contain serializable objects."));
	if (el.GetValue() != GetClassName().CStr())
	{
		throw ExceptionInvalidArgument(StringUTF8("bool Map::deserialize(xml::Element *el): ") + 
				_("Wrong XML element."));
	}
	protocols = Protocol(el.GetAttribute<int>("protocols"));
	Clear();
	for (xml::Element te = el.BeginElement(); te != el.EndElement(); ++te)
	{
		try
		{
			SObject d = DataFactory::CreateData(te);
			const StringUTF8 key = te.GetAttribute<StringUTF8>("key");
			data[key] = d;
		}
		catch (std::exception &e)
		{
			// XXX throw?
			CRNWarning(String(U"bool Map::deserialize(xml::Element &el): ") + 
				String(_("Cannot deserialize: ")) + te.GetValue() + String(_(" because ")) + e.what());
		}
	}
}

/*****************************************************************************/
/*!
 * Unsafe save
 *
 * \throws	ExceptionProtocol	the content of the map is not serializable
 * \param[in]	parent	the parent element to which we will add the new element
 * \return The newly created element
 */
xml::Element Map::serialize(xml::Element &parent) const
{
	if (!(protocols & crn::Protocol::Serializable))
		throw ExceptionProtocol(_("This map does not contain serializable objects."));
	xml::Element el(parent.PushBackElement(GetClassName().CStr()));
	el.SetAttribute("protocols", int(protocols));
	for (const_iterator tmp = Begin(); tmp != End(); ++tmp)
	{
		xml::Element te = (*tmp).second->Serialize(el);
		te.SetAttribute("key", (*tmp).first.CStr());
	}

	return el;
}

/*! 
 * Returns all keys
 * \return  a vector of keys
 */
std::set<String> Map::GetKeys() const
{
	std::set<String> keys;
	for (const_iterator tmp = Begin(); tmp != End(); ++tmp)
	{
		keys.insert(tmp->first);
	}
	return keys;
}

/*! 
 * Returns the first (lowest) key
 * \return the first (lowest) key or empty string if the map is empty
 */
String Map::FirstKey() const
{
	if (data.empty())
	{
		return U"";
	}
	return data.begin()->first;
}

/*! 
 * Returns the last (greatest) key 
 * \return the last (greatest) key or empty string if the map is empty
 */
String Map::LastKey() const
{
	if (data.empty())
	{
		return U"";
	}
	return data.rbegin()->first;
}

/*! 
 * Swaps contents with another map
 *
 * \param[in]	other	the map to swap with
 */
void Map::Swap(Map &other) noexcept
{
	data.swap(other.data);
	std::swap(protocols, other.protocols);
}

/*! 
 * Checks if the protocols are compatible with the object's constraints
 *
 * \param[in]  obj  the object to check
 * \return true if the protocols are compatible with the constraints, false else
 */
bool Map::checkProtocols(const Object &obj)
{
	if ((obj.GetClassProtocols() & protocols) == protocols)
	{
		return true;
	}
	try
	{
		const Map &m = static_cast<const Map&>(obj);
		if ((m.protocols & protocols) == protocols)
		{
			return true;
		}
	}
	catch (...) { }
	try
	{
		const Vector &v = static_cast<const Vector&>(obj);
		if ((v.GetContentProtocols() & protocols) == protocols)
		{
			return true;
		}
	}
	catch (...) { }
	return false;
}

CRN_BEGIN_CLASS_CONSTRUCTOR(Map)
	CRN_DATA_FACTORY_REGISTER(U"Map", Map)
CRN_END_CLASS_CONSTRUCTOR(Map)


