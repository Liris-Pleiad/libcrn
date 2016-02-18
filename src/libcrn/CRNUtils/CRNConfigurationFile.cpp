/* Copyright 2010-2016 CoReNum, INSA-Lyon
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
 * file: CRNConfigurationFile.cpp
 * \author Yann LEYDIER
 */

#include <CRNUtils/CRNConfigurationFile.h>
#include <CRNString.h>
#include <CRNData/CRNInt.h>
#include <CRNData/CRNReal.h>
#include <CRNException.h>
#include <CRNIO/CRNIO.h>
#include <CRNi18n.h>
#ifdef _MSC_VER
#	include <direct.h> // for getcwd
#	define getcwd _getcwd
#else
#	include <unistd.h>
#endif

using namespace crn;

/*! Constructor
 * \param[in]	application_name	the name of the application
 * \param[in]	file_name	the name of the configuration file. If empty, the application name is used
 */
ConfigurationFile::ConfigurationFile(const String &application_name, const StringUTF8 &file_name):
	appname(application_name),
	filename(file_name),
	data(std::make_shared<Map>(Protocol::Serializable))
{
	if (file_name.IsEmpty())
		filename = application_name.CStr();
}

/*! Loads the file from the following places
 * 	- current directory
 * 	- personal config directory (unix: $HOME/.config, win: %APPDATA%)
 * 	- system config directory (unix: /etc, win: %COMMONPROGRAMFILES%\\crn)
 * 	\return	the full path to the loaded file or an empty string if no file was found
 */
Path ConfigurationFile::Load()
{
	std::vector<Path> dirs, files;
	char cwd[4096];
	getcwd(cwd, 4096);
	dirs.push_back(cwd);
	dirs.push_back(GetUserDirectory());
	dirs.push_back(CRN_CONFIG_FULL_PATH);
	files.push_back(filename + ".xml");
	files.push_back("_" + filename + ".xml");
	files.push_back("." + filename + ".xml");

	data->Clear();
	for (auto idir = dirs.begin(); (idir != dirs.end()); ++idir)
	{
		for (auto ifn = files.begin(); (ifn != files.end()); ++ifn)
		{
			Path confname(*idir);
			confname += Path::Separator();
			confname += *ifn;
			CRNdout << confname.CStr() << std::endl;
			try
			{
				data->Load(confname);
				CRNdout << "Configuration loaded from: " << confname.CStr() << std::endl;
				return confname;
			}
			catch (...)
			{ }
		}
	}
	return "";
}

/*! Saves the file to the user's personal space (unix: $HOME/.config, win: %APPDATA%)
 * \return	the full path to the written file or an empty string if no file could not be saved
 */
Path ConfigurationFile::Save()
{
	Path fname(GetUserDirectory() / filename + ".xml");
	try
	{
		data->Save(fname);
		return fname;
	}
	catch (...)
	{
		return "";
	}
}

/*! Gets a value. If the key does not exist, creates a key/value pair.
 * \param[in]	key	the key of the value to get
 * \return	a reference to the pointer to the value
 */
SObject& ConfigurationFile::operator[](const String &key)
{
	return (*data)[key];
}

/*! Gets a value
 * \param[in]	key	the key of the value to get
 * \return	a pointer to the value or nullptr if not found
 */
SObject ConfigurationFile::GetData(const String &key)
{
	auto it(data->Find(key));
	if (it != data->end())
		return it->second;
	else
		return nullptr;
}

/*! Gets a value
 * \param[in]	key	the key of the value to get
 * \return	a pointer to the value or nullptr if not found
 */
SCObject ConfigurationFile::GetData(const String &key) const
{
	Map::const_iterator it(data->Find(key));
	if (it != data->end())
		return it->second;
	else
		return nullptr;
}

/*! Gets a path
 * \throws	ExceptionInvalidArgument	key does not point to a Path
 * \param[in]	key	the key of the value to get
 * \return	a path or ExceptionUninitialized if not found or value is of wrong type
 */
Path ConfigurationFile::GetPath(const String &key) const
{
	SCPath str(std::dynamic_pointer_cast<const Path>(GetData(key)));
	if (str)
		return *str;
	throw ExceptionInvalidArgument(key.CStr() + StringUTF8(_(" is not a Path.")));
}

/*! Gets a string
 * \throws	ExceptionInvalidArgument	key does not point to a String
 * \param[in]	key	the key of the value to get
 * \return	a string or ExceptionUninitialized if not found or value is of wrong type
 */
String ConfigurationFile::GetString(const String &key) const
{
	SCString str(std::dynamic_pointer_cast<const String>(GetData(key)));
	if (str)
		return *str;
	throw ExceptionInvalidArgument(key.CStr() + StringUTF8(_(" is not a String.")));
}

/*! Gets a utf8 string
 * \throws	ExceptionInvalidArgument	key does not point to a StringUTF8
 * \param[in]	key	the key of the value to get
 * \return	a utf8 string or ExceptionUninitialized if not found or value is of wrong type
 */
StringUTF8 ConfigurationFile::GetStringUTF8(const String &key) const
{
	SCStringUTF8 str(std::dynamic_pointer_cast<const StringUTF8>(GetData(key)));
	if (str)
		return *str;
	throw ExceptionInvalidArgument(key.CStr() + StringUTF8(_(" is not a StringUTF8.")));
}

/*! Gets a Prop3
 * \throws	ExceptionInvalidArgument	key does not point to a Prop3
 * \param[in]	key	the key of the value to get
 * \return	a Prop3 or ExceptionUninitialized if not found or value is of wrong type
 */
Prop3 ConfigurationFile::GetProp3(const String &key) const
{
	SCProp3 val(std::dynamic_pointer_cast<const Prop3>(GetData(key)));
	if (val)
		return *val;
	throw ExceptionInvalidArgument(key.CStr() + StringUTF8(_(" is not a Prop3.")));
}

/*! Gets an int
 * \throws	ExceptionInvalidArgument	key does not point to an int
 * \param[in]	key	the key of the value to get
 * \return	an int or ExceptionUninitialized if not found or value is of wrong type
 */
int ConfigurationFile::GetInt(const String &key) const
{
	SCInt val(std::dynamic_pointer_cast<const Int>(GetData(key)));
	if (val)
		return val->GetValue();
	throw ExceptionInvalidArgument(key.CStr() + StringUTF8(_(" is not an int.")));
}

/*! Gets a double
 * \throws	ExceptionInvalidArgument	key does not point to a double
 * \param[in]	key	the key of the value to get
 * \return	a double or ExceptionUninitialized if not found or value is of wrong type
 */
double ConfigurationFile::GetDouble(const String &key) const
{
	SCReal val(std::dynamic_pointer_cast<const Real>(GetData(key)));
	if (val)
		return val->GetValue();
	throw ExceptionInvalidArgument(key.CStr() + StringUTF8(_(" is not a double.")));
}

/*! Returns the path to the user configuration directory
 * \return	%APPDATA%\<appname> on Windows, $HOME/.config/<appname> else
 */
Path ConfigurationFile::GetUserDirectory() const
{
#ifdef CRN_PF_WIN32
	crn::Path p(getenv("APPDATA"));
#else
	crn::Path p(getenv("HOME"));
	p /= ".config";
#endif
	p /= appname;
	if (!crn::IO::Access(p, crn::IO::EXISTS))
	{
		try
		{
			crn::IO::Mkdir(p);
		}
		catch (...)
		{
			return "";
		}
	}
	return p;
}

/*! \cond */
CRN_BEGIN_CLASS_CONSTRUCTOR(ConfigurationFile)
	String::Initialize();
	StringUTF8::Initialize();
	Path::Initialize();
	Int::Initialize();
	Real::Initialize();
	Prop3::Initialize();
	Map::Initialize();
CRN_END_CLASS_CONSTRUCTOR(ConfigurationFile)
/*! \endcond */

