/* Copyright 2011 CoReNum
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
 * file: CRNFileShield.h
 * \author Yann LEYDIER
 */

#ifndef CRNFileShield_HEADER
#define CRNFileShield_HEADER

#include <CRNIO/CRNPath.h>
#include <mutex>
#include <map>

namespace crn
{
	/****************************************************************************/
	/*! \brief A mutex manager for file access
	 *
	 * A mutex manager for file access.
	 *
	 * Usage: place the following line at the beginning of the block you want to protect
	 * \code
	 * std::lock_guard<std::mutex> lock(crn::FileShield::GetMutex("/path/to/file"));
	 * \endcode
	 *
	 * \author 	Yann LEYDIER
	 * \date		26 Jan 2011
	 * \version 0.1
	 * \ingroup io
	 */
	class FileShield
	{
		public:
			/*! \brief Gets the mutex associated to a file */
			static std::mutex& GetMutex(const Path &fname);

		private:
			/*! \brief Singleton instance */
			static FileShield& getInstance();

			/*! \brief Constructor */
			FileShield();
			std::map<Path, std::unique_ptr<std::mutex> > shields; /*!< list of mutex */
	};
}


#endif
