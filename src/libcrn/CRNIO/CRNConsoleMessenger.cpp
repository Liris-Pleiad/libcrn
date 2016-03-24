/* Copyright 2009-2016 INSA Lyon, ENS-Lyon
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
 * file: CRNConsoleMessenger.cpp
 * \author Yann LEYDIER
 */

#include <CRNIO/CRNConsoleMessenger.h>
#ifdef CRN_C_CONSOLE_MESSENGER
#	include <stdio.h>
#else
#	include <iostream>
#endif
#include <CRNi18n.h>

using namespace crn;

/*! Default constructor */
ConsoleMessenger::ConsoleMessenger()
{
	DebugPrefix = String(U"[") + _("DEBUG") + String(U"] ");
	VerbosePrefix = U"[libcrn] ";
	WarningPrefix = String(U"[") + _("WARNING") + String(U"] ");
	ErrorPrefix = String(U"[") + _("ERROR") + String(U"] ");
}

ConsoleMessenger::~ConsoleMessenger() = default;

/*! Prints a debug message */
void ConsoleMessenger::PrintDebug(const String &msg)
{
#ifdef CRN_C_CONSOLE_MESSENGER
	printf("%s%s\n", DebugPrefix.CStr(), msg.CStr());
#else
	std::cout << DebugPrefix.CStr() << msg.CStr() << std::endl;
#endif
}

/*! Prints a message */
void ConsoleMessenger::PrintVerbose(const String &msg)
{
#ifdef CRN_C_CONSOLE_MESSENGER
	printf("%s%s\n", VerbosePrefix.CStr(), msg.CStr());
#else
	std::cout << VerbosePrefix.CStr() << msg.CStr() << std::endl;
#endif
}

/*! Prints a warning message */
void ConsoleMessenger::PrintWarning(const String &msg)
{
#ifdef CRN_C_CONSOLE_MESSENGER
	printf("%s%s\n", WarningPrefix.CStr(), msg.CStr());
#else
	std::cout << WarningPrefix.CStr() << msg.CStr() << std::endl;
#endif
}

/*! Prints an error message */
void ConsoleMessenger::PrintError(const String &msg)
{
#ifdef CRN_C_CONSOLE_MESSENGER
	printf("%s%s\n", ErrorPrefix.CStr(), msg.CStr());
#else
	std::cerr << ErrorPrefix.CStr() << msg.CStr() << std::endl;
#endif
}


