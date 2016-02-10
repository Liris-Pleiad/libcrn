/* Copyright 2008-2015 INSA Lyon, CoReNum
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
 * file: CRNImageLuv.h
 * \author Loris EYNARD, Yann LEYDIER
 */

#ifndef CRNIMAGELUV_HEADER
#define CRNIMAGELUV_HEADER

#include <CRNImage/CRNImageFormats.h>

namespace crn
{
	/*! \addtogroup imageother */
	/*@{*/
	ImageDoubleGray LChannel(const ImageLuv &img);
	ImageDoubleGray UChannel(const ImageLuv &img);
	ImageDoubleGray VChannel(const ImageLuv &img);
	/*@}*/
}

#endif

