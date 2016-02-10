/* Copyright 2011-2015 CoReNum, INSA-Lyon
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
 * file: CRNRWLock.h
 * \author Yann LEYDIER
 */

#ifndef CRNRWLock_HEADER
#define CRNRWLock_HEADER

#include <CRN.h>

namespace crn
{
	/*! \brief A lock that allows multiple readers or one exclusive writer
	 *
	 * A lock that allows multiple readers or one exclusive writer
	 *
	 * \ingroup utils
	 * \date	Oct 2011
	 * \author Yann LEYDIER
	 */
	class RWLock
	{
		public:
			/*! \brief Constructor */
			RWLock();
			/*! \brief Destructor */
			~RWLock();
			RWLock(const RWLock &) = delete;
			RWLock(RWLock&&) = delete;
			RWLock& operator=(const RWLock&) = delete;
			RWLock& operator=(RWLock&&) = delete;

			/*! \brief Requests authorization to read */
			void WaitReadLock();
			/*! \brief Releases read token */
			void ReadUnlock();

			/*! \brief Requests authorization to write */
			void WaitWriteLock();
			/*! \brief Releases write token */
			void WriteUnlock();
			
			/*! \brief RA2I read auto-lock */
			class ReadLock
			{
				public:
					/*! \brief Locks */
					ReadLock(RWLock &l):lock(l) { lock.WaitReadLock(); }
					/*! \brief Unlocks */
					~ReadLock() { lock.ReadUnlock(); }
				private:
					RWLock &lock;
			};

			/*! \brief RA2I write auto-lock */
			class WriteLock
			{
				public:
					/*! \brief Locks */
					WriteLock(RWLock &l):lock(l) { lock.WaitWriteLock(); }
					/*! \brief Unlocks */
					~WriteLock() { lock.WriteUnlock(); }
				private:
					RWLock &lock;
			};

		private:
			struct internal_data;
			std::unique_ptr<internal_data> data;
	};
}

#endif

