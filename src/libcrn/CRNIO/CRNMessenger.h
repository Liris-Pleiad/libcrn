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
 * file: CRNMessenger.h
 * \author Yann LEYDIER
 */

#ifndef CRNMessenger_HEADER
#define CRNMessenger_HEADER

#include <CRNString.h>

namespace crn
{
	/*! \brief Base class for an object that prints messages
	 *
	 * Base class for an object that prints messages
	 * \author	Yann LEYDIER
	 * \date	August 2009
	 * \version 0.1
	 * \ingroup io
	 */
	class Messenger
	{
		public:
			/*! \brief Empty constructor */
			Messenger() { }
			/*! \brief Empty virtual destructor */
			virtual ~Messenger() { }
			/*! \brief Prints a debug message */
			virtual void PrintDebug(const String &msg) = 0;
			/*! \brief Prints a message */
			virtual void PrintVerbose(const String &msg) = 0;
			/*! \brief Prints a warning message */
			virtual void PrintWarning(const String &msg) = 0;
			/*! \brief Prints an error message */
			virtual void PrintError(const String &msg) = 0;

			String DebugPrefix, VerbosePrefix, WarningPrefix, ErrorPrefix; /*!< Optional prefixes for message printing */
		private:
			/*! \brief Disable copy constructor */
			Messenger(const Messenger &);
			/*! \brief Disable copy operator */
			Messenger& operator=(const Messenger &);

	};
	CRN_ALIAS_SMART_PTR(Messenger)
}

#endif


