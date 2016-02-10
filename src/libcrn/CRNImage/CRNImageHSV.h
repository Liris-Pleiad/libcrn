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
 * file: CRNImageHSV.h
 * \author Yann LEYDIER
 */

#ifndef CRNIMAGEHSV_HEADER
#define CRNIMAGEHSV_HEADER

#include <CRNImage/CRNImage.h>

namespace crn
{
	/*! \addtogroup imageother */
	/*@{*/
	/*! \brief Create an image from the hue channel */
	ImageAngle HChannel(const ImageHSV &img);
	/*! \brief Create an image from the saturation channel */
	ImageGray SChannel(const ImageHSV &img);
	/*! \brief Create an image from the value channel */
	ImageGray VChannel(const ImageHSV &img);
	/*! \brief Create an image from the red channel */
	ImageGray RedChannel(const ImageHSV &img);
	/*! \brief Create an image from the green channel */
	ImageGray GreenChannel(const ImageHSV &img);
	/*! \brief Create an image from the blue channel */
	ImageGray BlueChannel(const ImageHSV &img);
	/*@}*/
}

#endif

