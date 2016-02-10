/* Copyright 2007-2015 Yann LEYDIER, CoReNum, INSA-Lyon
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
 * file: CRNImageAngle.h
 * \author Yann LEYDIER
 */

#ifndef CRNIMAGEANGLE_HEADER
#define CRNIMAGEANGLE_HEADER

#include <CRNImage/CRNImageFormats.h>

namespace crn
{
	/*! \brief	Transforms an angle image into a RGB image */
	ImageRGB ColorWheel(const ImageAngle &img);
}
#endif
