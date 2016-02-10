/* Copyright 2009-2014 Orange Labs, INSA-Lyon
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
 * file: CRNFileMessenger.cpp
 * \author Guillaume ETIEVENT, Yann LEYDIER
 */

#include <CRNException.h>
#include <CRNIO/CRNFileMessenger.h>
#include <iostream>
#include <CRNi18n.h>

using namespace crn;

/*! 
 * Constructor with file name
 * \throws	ExceptionIO	cannot open file
 * \param[in]	filePath	path to the file
 */
FileMessenger::FileMessenger(const Path& filePath): _filePath("")
{
	//Try opening file
	_log.open(filePath.CStr());
	if (!_log.is_open()) {
		throw ExceptionIO("Error while opening log file at " + StringUTF8(filePath));
	}
	_filePath = filePath;

	DebugPrefix   = String(U"[libcrn ") + _("DEBUG")   + String(U"] ");
	VerbosePrefix = String(U"[libcrn ") + _("VERBOSE") + String(U"] ");
	WarningPrefix = String(U"[libcrn ") + _("WARNING") + String(U"] ");
	ErrorPrefix   = String(U"[libcrn ") + _("ERROR")   + String(U"] ");
}

/*! Destructor */
FileMessenger::~FileMessenger()
{
	if (_log.is_open()) {
		_log.close();
	}
}

/*! Prints a debug message */
void FileMessenger::PrintDebug(const String &msg)
{
	if (_log.is_open()) {
		_log << DebugPrefix.CStr() << msg.CStr() << std::endl;
	}
}

/*! Prints a message */
void FileMessenger::PrintVerbose(const String &msg)
{
	if (_log.is_open()) {
		_log << VerbosePrefix.CStr() << msg.CStr() << std::endl;
	}
}

/*! Prints a warning message */
void FileMessenger::PrintWarning(const String &msg)
{
	if (_log.is_open()) {
		_log << WarningPrefix.CStr() << msg.CStr() << std::endl;
	}
}

/*! Prints an error message */
void FileMessenger::PrintError(const String &msg)
{
	if (_log.is_open()) {
		_log << ErrorPrefix.CStr() << msg.CStr() << std::endl;
	}
}


