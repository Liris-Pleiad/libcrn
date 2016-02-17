/* Copyright 2006-2012 Yann LEYDIER, CoReNum, INSA Lyon
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
 * file: CRNIO.h
 * \author Yann LEYDIER
 */

#ifndef CRNIO_HEADER
#define CRNIO_HEADER

#include <CRNString.h>
#include <CRNIO/CRNPath.h>
#ifdef _MSC_VER
#	include <io.h>
#endif
#include <CRNIO/CRNMessenger.h>

/*! \defgroup io Input/Output */

namespace crn
{
	/*! \brief General purpose IO class
	 *
	 * This class contains methods to send messages to the end-user via the console or Gtk objects.
	 * \author	Yann LEYDIER
	 * \date	June 2009
	 * \ingroup io
	 */
	class IO
	{
		/*************************************************************************/
		/* Messages **************************************************************/
		/*************************************************************************/
		public:
			/*! \brief Prints debug messages. Please use CRNDebug() macro instead. */
			static void Debug(const String &msg);
			/*! \brief Prints warning messages. Please use CRNWarning() macro instead. */
			static void Warning(const String &msg);
			/*! \brief Prints verbose messages. Please use CRNVerbose() macro instead. */
			static void Verbose(const String &msg);
			/*! \brief Prints error messages. Please use CRNError() macro instead. */
			static void Error(const String &msg);

			/*! \brief Delegate that will print the messages */
			static std::shared_ptr<Messenger>& CurrentMessenger();
			
			/*! \brief If true, Debug, Warning, Verbose and Error don't print anything. */
			static bool& IsQuiet(); 
			/*! \brief Controls whether CRNVerbose prints something or not */
			static bool& IsVerbose(); 

		/*************************************************************************/
		/* Files and directories *************************************************/
		/*************************************************************************/
		public:
			/*! \brief Creates a directory */
			static void Mkdir(const Path &name);
			/*! \brief Removes a file */
			static void Rm(const Path &name);
			/*! \brief Removes a file and protects it with mutex */
			static void ShieldRm(const Path &name);
			/*! \brief Recursively removes a directory */
			static void Rmdir(const Path &name);
			/*! \brief IO modes (use with IO::Access()) */
			enum { 
				EXISTS = 
#ifdef F_OK
					F_OK,
#else
					0,
#endif
				READ = 
#ifdef R_OK
					R_OK,
#else
					4,
#endif
				WRITE =
#ifdef W_OK
					W_OK,
#else
					2,
#endif
				EXECUTE = 
#ifdef X_OK
					X_OK
#else
					1
#endif
		 	};
			/*! \brief Checks rights on a file */
			static bool Access(const Path &name, int mode);
			/*! \brief Copies a file */
			static void Copy(const Path &src, const Path &dst);
			/*! \brief Copies a file and protects source and destination with mutex */
			static void ShieldCopy(const Path &src, const Path &dst);

			/*! \brief A handler to the content of a directory
			 *
			 * A Directory object contains a list of files and a list of subdirectories
			 */
			class Directory
			{
				public:
					/*! \brief Constructor */
					Directory(const Path &path);
					/*! \brief Returns the list of files */
					inline const std::vector<Path>& GetFiles() const { return files; }
					/*! \brief Returns the list of directories */
					inline const std::vector<Path>& GetDirs() const { return directories; }

				private:
					std::vector<Path> files; /*!< the files contained if the directory*/
					std::vector<Path> directories; /*!< the subdirectories contained if the directory */
			};

	};
}

/*! \addtogroup io */
/*@{*/
#ifdef DEBUG
#	define CRNDebug(x) crn::IO::Debug(x)
# include <iostream>
#	define CRNdout std::cout 
#else
#	define CRNDebug(x)
# include <fstream>
#	define CRNdout std::ofstream()
#endif
#define CRNWarning(x) crn::IO::Warning(x)
#define CRNVerbose(x) crn::IO::Verbose(x)
#define CRNError(x) crn::IO::Error(x)
#ifdef PRINTOPTIM
#	define CRNOptimNeeded(x) crn::IO::Warning(String(U"Optimization needed: ") + x)
#else
#	define CRNOptimNeeded(x) 
#endif
/*@}*/
#endif


