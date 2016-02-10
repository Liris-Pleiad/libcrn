/* Copyright 2009-2015 Orange Labs, CoReNum, Université Paris Descartes
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
 * \author Guillaume ETIEVENT, Yann LEYDIER
 */

#ifndef CRNFileMessenger_HEADER
#define CRNFileMessenger_HEADER

#include <fstream>
#include <CRNIO/CRNMessenger.h>
#include <CRNIO/CRNPath.h>

namespace crn
{
	/*! \brief Class to print messages to a file
	 *
	 * Class to print messages to a file
	 * \author	Guillaume ETIEVENT
	 * \date	March 2010
	 * \ingroup io
	 */
	class FileMessenger: public Messenger
	{
		public:
			/*! \brief Constructor with filename */
			FileMessenger(const Path& filePath);
			/*! \brief Destructor */
			virtual ~FileMessenger() override;
			/*! \brief Prints a debug message */
			virtual void PrintDebug(const String &msg) override;
			/*! \brief Prints a message */
			virtual void PrintVerbose(const String &msg) override;
			/*! \brief Prints a warning message */
			virtual void PrintWarning(const String &msg) override;
			/*! \brief Prints an error message */
			virtual void PrintError(const String &msg) override;

		protected:
			/*! \brief Stream to write in */
			std::ofstream _log;
			/*! \brief Path of the file to be written */
			Path _filePath;
	};
}

#endif


