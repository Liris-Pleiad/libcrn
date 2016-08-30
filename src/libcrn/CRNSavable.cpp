/* Copyright 2006-2016 INSA Lyon, CoReNum, ENS-Lyon
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
 * file: CRNSavable.cpp
 * \author Yann LEYDIER
 */

#include <CRNi18n.h>
#include <CRNSavable.h>
#include <CRNException.h>
#include <CRNData/CRNMap.h>
#include <CRNUtils/CRNXml.h>

using namespace crn;

const StringUTF8 USERDATA_NAME("userdata");

/*!
 * Default constructor
 * \param[in]	s	the name of the object
 */
Savable::Savable(const String &s):
	name(s),
	user_data(nullptr),
	filelock(std::make_unique<std::mutex>()),
	filename("")
{ 
}

/*!
 * Destructor. Frees the user data.
 */
Savable::~Savable()
{ 
}

Savable::Savable(Savable&&) noexcept = default;
Savable& Savable::operator=(Savable&&c) = default;

/*****************************************************************************/
/*!
 * Tests if a user data key exists
 *
 * \param[in]	key	The data key
 * \return	true if the key exists, false else
 */
bool Savable::IsUserData(const String &key) const
{
	if (!user_data) 
		return false;
	if (user_data->Find(key) != user_data->end())
		return true;
	else
		return false;
}

/*****************************************************************************/
/*!
 * Gets a user data by key
 *
 * \param[in]	key	The data key
 * \return	the value or nullptr if key does not exist
 */
SObject Savable::GetUserData(const String &key)
{
	if (!user_data)
		return nullptr;
	if (IsUserData(key))
		return (*user_data)[key];
	else
		return nullptr;
}

/*****************************************************************************/
/*!
 * Gets a user data key by value
 *
 * \param[in]	value	The value to search
 * \return	The key corresponding to the value, or "" if not found
 */
String Savable::GetUserDataKey(const SObject &value) const
{
	if (!user_data)
		return U"";
	for (Map::const_iterator it = user_data->begin(); it != user_data->end(); ++it)
	{
		if (it->second == value)
			return it->first;
	}
	return U"";
}

/*****************************************************************************/
/*!
 * Gets a user data by key
 *
 * \param[in]	key	The data key
 * \return	the value or nullptr if key does not exist
 */
const SObject Savable::GetUserData(const String &key) const
{
	if (!user_data)
		return nullptr;
	if (IsUserData(key))
		return (*user_data)[key];
	else
		return nullptr;
}

/*****************************************************************************/
/*!
 * Deletes a user data entry and frees the value
 *
 * \throws	ExceptionNotFound	key not found
 *
 * \param[in]	key	The data key
 */
void Savable::DeleteUserData(const String &key)
{
	if (!user_data)
		throw ExceptionNotFound(_("No user data to remove."));
	user_data->Remove(key); // may throw
}

/*!
 * Adds an object to the user data
 *
 * \param[in]	key	the (unique) key of the data
 * \param[in]	value	the data to add
 */
void Savable::SetUserData(const String &key, SObject value)
{
	if (!user_data)
		user_data.reset(new Map());
	user_data->Set(key, value);
}

/*! 
 * Deletes all user data entries
 */
void Savable::ClearUserData()
{
	if (user_data)
		user_data->Clear();
}

/***************************************************************************/
/* Savable informal protocol                                               */
/***************************************************************************/
/*!
 * Constructor from file name. Does not load the file!
 *
 * \param[in]	s	the name of the object
 * \param[in]	fname	the file name
 */
Savable::Savable(const String &s, const Path &fname):
	name(s),
	user_data(nullptr),
	filelock(std::make_unique<std::mutex>()),
	filename(fname)
{ 
}

/*!
 * Loads the object from an XML file. Safe.
 *
 * \throws	ExceptionIO	cannot read file
 * \throws	ExceptionUninitialized	empty file
 * \throws	ExceptionNotFound	invalid file
 * \throws	ExceptionProtocol	load unimplemented
 *
 * \param[in]	fname	the file name
 */
void Savable::Load(const Path &fname)
{
	std::lock_guard<std::mutex> lock(*filelock);
	Path fn;
	if (fname.IsAbsolute())
		fn = fname;
	else
		fn = completeFilename(fname);
	load(fn);
	filename = fname;
}

/*!
 * Saves the object to an XML file. Safe.
 *
 * \throws	ExceptionIO	cannot write file
 * \throws	ExceptionProtocol	save unimplemented
 *
 * \param[in]	fname	the file name
 */
void Savable::Save(const Path &fname)
{
	std::lock_guard<std::mutex> lock(*filelock);
	Path fn;
	if (fname.IsAbsolute())
		fn = fname;
	else
		fn = completeFilename(fname);
	save(fn);
	filename = fname;
}

/*!
 * Saves the object to an already set XML file.
 *
 * \throws	ExceptionUninitialized	no filename
 * \throws	ExceptionIO	cannot write file
 * \throws	ExceptionProtocol	save unimplemented
 */
void Savable::Save()
{
	if (filename.IsEmpty())
		return throw ExceptionUninitialized(_("Cannot save an object with no filename."));
	Save(filename);
}

/*!
 * Completes a relative file name with the default path.
 *
 * \param[in]	fn the relative file name
 *
 * \return	the absolute file name
 */
Path Savable::completeFilename(const Path &fn) const
{
	return fn;
}

/*!
 * Loads the object from an XML file. Unsafe.
 *
 * \throws	ExceptionIO	cannot read file
 * \throws	ExceptionUninitialized	empty file
 * \throws	ExceptionNotFound	invalid file
 * \throws	ExceptionProtocol	load unimplemented
 *
 * \param[in]	fname	the file name
 *
 * \return	true if success, false else.
 */
void Savable::load(const Path &fname)
{
	throw ExceptionProtocol(StringUTF8("load() not implemented in ") + typeid(*this).name());
}

/*!
 * Saves the object to an XML file. Unsafe.
 *
 * \throws	ExceptionIO	cannot write file
 * \throws	ExceptionProtocol	save unimplemented
 *
 * \param[in]	fname	the file name
 */
void Savable::save(const Path &fname)
{
	throw ExceptionProtocol(StringUTF8("save() not implemented in ") + typeid(*this).name());
}

/*****************************************************************************/
/*! 
 * Internal. Initializes some internal data from an XML element. 
 *
 * \param[in]	el	the element that contains the serialized object
 */
void Savable::deserialize_internal_data(xml::Element &el)
{
	// read name if any
	const StringUTF8 nam = el.GetAttribute<StringUTF8>("name");

	xml::Element udel(el.GetFirstChildElement("Map"));
	while (udel)
	{ // look for a map containing user data
		const StringUTF8 role = udel.GetAttribute<StringUTF8>("role");
		// check role
		if (role == USERDATA_NAME)
		{ // found the user data
			if (!user_data)
			{ // create the object if needed
				user_data.reset(new Map());
			}
			else
			{ // clear the current user data if needed
				user_data->Clear();
			}
			// read the user data and quit the method
			user_data->Deserialize(udel);
			return;
		}
		udel = udel.GetNextSiblingElement("Map");
	}
}

/*****************************************************************************/
/*! 
 * Internal. Dumps some internal data to an XML element.
 *
 * \param[in]	el	the element that contains the serialized object
 */
void Savable::serialize_internal_data(xml::Element &el) const
{
	el.SetAttribute("name", name.CStr());
	if (user_data)
	{
		xml::Element udel = user_data->Serialize(el);
		udel.SetAttribute("role", USERDATA_NAME);
	}
}


