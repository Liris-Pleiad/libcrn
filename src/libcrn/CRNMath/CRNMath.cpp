/* Copyright 2009-2013 INSA Lyon
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
 * file: CRNMath.cpp
 * \author Yann LEYDIER
 */

#include <CRNMath/CRNMath.h>

using namespace crn;

/*!
 * Constructor from direction
 *
 * \param[in]  d  the direction to import. LEFT and RIGHT map to HORIZONTAL. TOP and BOTTOM map to VERTICAL. Others map to INVALID.
 */
Orientation crn::ToOrientation(Direction d) noexcept
{
	if (!!(d & Direction::LEFT) || !!(d & Direction::RIGHT))
	{
		return Orientation::HORIZONTAL;
	}
	else if (!!(d & Direction::TOP) || !!(d & Direction::BOTTOM))
	{
		return Orientation::VERTICAL;
	}
	else
		return Orientation::INVALID;
}

