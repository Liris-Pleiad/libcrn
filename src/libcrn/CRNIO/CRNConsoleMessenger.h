/* Copyright 2009 INSA Lyon
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
 * file: CRNConsoleMessenger.h
 * \author Yann LEYDIER
 */

#ifndef CRNConsoleMessenger_HEADER
#define CRNConsoleMessenger_HEADER

#include <CRNIO/CRNMessenger.h>

namespace crn
{
	/*! \brief Class to print messages to the console
	 *
	 * Class to print messages to the console
	 * \author	Yann LEYDIER
	 * \date	August 2009
	 * \ingroup io
	 */
	class ConsoleMessenger: public Messenger
	{
		public:
			/*! \brief Default constructor */
			ConsoleMessenger();
			virtual ~ConsoleMessenger() override {}
			/*! \brief Prints a debug message */
			virtual void PrintDebug(const String &msg) override;
			/*! \brief Prints a message */
			virtual void PrintVerbose(const String &msg) override;
			/*! \brief Prints a warning message */
			virtual void PrintWarning(const String &msg) override;
			/*! \brief Prints an error message */
			virtual void PrintError(const String &msg) override;
	};
}

#endif


