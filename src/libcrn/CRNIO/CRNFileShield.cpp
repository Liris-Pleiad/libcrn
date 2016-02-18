/* Copyright 2011-2016 CoReNum, INSA-Lyon
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
 * file: CRNFileShield.cpp
 * \author Yann LEYDIER
 */

#include <CRNIO/CRNFileShield.h>

using namespace crn;

/*! Constructor */
FileShield::FileShield()
{
}

/*! Singleton instance
 * \return	the single instance
 */
FileShield& FileShield::getInstance()
{
	static FileShield fs;
	return fs;
}

/*! Gets the mutex associated to a file
 * \param[in]	fname	the file to shield
 * \return	a reference to the mutex associated to the file
 */
std::mutex& FileShield::GetMutex(const Path &fname)
{
	FileShield &fs(getInstance());
	auto it = fs.shields.find(fname);
	if (it == fs.shields.end())
	{
		
		return *(fs.shields.insert(std::make_pair(fname, std::unique_ptr<std::mutex>(new std::mutex))).first->second);
	}
	else
	{
		return *(it->second);
	}
}

