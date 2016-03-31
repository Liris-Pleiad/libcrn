/* Copyright 2011-2012 CoReNum, INSA-Lyon
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
 * file: CRNRWLock.cpp
 * \author Yann LEYDIER
 */

#include <CRNUtils/CRNRWLock.h>
#ifdef _MSC_VER
#	include <windows.h>
#else
#	include <pthread.h>
#endif

using namespace crn;

struct RWLock::internal_data
{
#ifdef _MSC_VER
	bool wrpriority;              // true, if writer priority
	DWORD rdcount;                // number of active readers
	DWORD rdwaiting;              // number of waiting readers
	DWORD wrcount;                // number of active writers
	DWORD wrwaiting;              // number of waiting writers
	HANDLE rdgreen, wrgreen;      // reader and writer events
	CRITICAL_SECTION rwcs;        // R/W lock critical section
#else
	pthread_rwlock_t rwlock;
#endif
};

/*! Constructor */
RWLock::RWLock():
	data(std::make_unique<internal_data>())
{
#ifdef _MSC_VER
	data->wrpriority = true; // XXX
	data->rdcount = data->rdwaiting = data->wrcount = data->wrwaiting = 0;
	InitializeCriticalSection(&(data->rwcs));
	data->rdgreen = CreateEvent(nullptr, FALSE, TRUE, nullptr);
	data->wrgreen = CreateEvent(nullptr, FALSE, TRUE, nullptr);
#else
	pthread_rwlock_init(&(data->rwlock), nullptr);
#endif
}

/*! Destructor */
RWLock::~RWLock()
{
#ifdef _MSC_VER
	CloseHandle(data->rdgreen);
	CloseHandle(data->wrgreen);
	DeleteCriticalSection(&(data->rwcs));
#else
	pthread_rwlock_destroy(&(data->rwlock));
#endif
}

/*! Requests authorization to read */
void RWLock::WaitReadLock()
{
#ifdef _MSC_VER
	bool wait = false;
	do 
	{
		EnterCriticalSection(&(data->rwcs));
		//
		// acquire lock if 
		//   - there are no active writers and 
		//     - readers have priority or
		//     - writers have priority and there are no waiting writers
		//
		if (!data->wrcount && (!data->wrpriority || !data->wrwaiting))
		{
			if (wait)
			{
				data->rdwaiting -= 1;
				wait = false;
			}
			data->rdcount += 1;
		}
		else
		{
			if (!wait)
			{
				data->rdwaiting += 1;
				wait = true;
			}
			// always reset the event to avoid 100% CPU usage
			ResetEvent(data->rdgreen);
		}

		LeaveCriticalSection(&(data->rwcs));

		if (wait)
		{
			WaitForSingleObject(data->rdgreen, INFINITE);
		}

	} while(wait);
#else
	pthread_rwlock_rdlock(&(data->rwlock));
#endif
}

/*! Releases read token */
void RWLock::ReadUnlock()
{
#ifdef _MSC_VER
	EnterCriticalSection(&(data->rwcs));
	data->rdcount -= 1;

	// always release waiting threads (do not check for rdcount == 0)
	if (data->wrpriority)
	{
		if (data->wrwaiting)
			SetEvent(data->wrgreen);
		else if(data->rdwaiting)
			SetEvent(data->rdgreen);
	}
	else
	{
		if(data->rdwaiting)
			SetEvent(data->rdgreen);
		else if(data->wrwaiting)
			SetEvent(data->wrgreen);
	}
	LeaveCriticalSection(&(data->rwcs));
#else
	pthread_rwlock_unlock(&(data->rwlock));
#endif
}

/*! Requests authorization to write */
void RWLock::WaitWriteLock()
{
#ifdef _MSC_VER
	bool wait = false;
	do
	{
		EnterCriticalSection(&(data->rwcs));
		//
		// acquire lock if 
		//   - there are no active readers nor writers and 
		//     - writers have priority or
		//     - readers have priority and there are no waiting readers
		//
		if (!data->rdcount && !data->wrcount && (data->wrpriority || !data->rdwaiting))
		{
			if (wait)
			{
				data->wrwaiting -= 1;
				wait = false;
			}
			data->wrcount++;
		}
		else
		{
			if(!wait)
			{
				data->wrwaiting += 1;
				wait = true;
			}
			// always reset the event to avoid 100% CPU usage
			ResetEvent(data->wrgreen);
		}

		LeaveCriticalSection(&(data->rwcs));

		if (wait)
		{
			WaitForSingleObject(data->wrgreen, INFINITE);
		}
	} while (wait);
#else
	pthread_rwlock_wrlock(&(data->rwlock));
#endif
}

/*! Releases write token */
void RWLock::WriteUnlock()
{
#ifdef _MSC_VER
	EnterCriticalSection(&(data->rwcs));
	data->wrcount -= 1;
	if (data->wrpriority)
	{
		if (data->wrwaiting)
			SetEvent(data->wrgreen);
		else if (data->rdwaiting)
			SetEvent(data->rdgreen);
	}
	else
	{
		if (data->rdwaiting)
			SetEvent(data->rdgreen);
		else if (data->wrwaiting)
			SetEvent(data->wrgreen);
	}
	LeaveCriticalSection(&(data->rwcs));
#else
	pthread_rwlock_unlock(&(data->rwlock));
#endif
}

