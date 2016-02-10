/* Copyright 2007-2015 Yann LEYDIER, INSA-Lyon
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
 * file: CRNImageAngle.cpp
 * \author Yann LEYDIER
 */

#include <CRNImage/CRNImageAngle.h>
#include <CRNImage/CRNImage.h>
#include <CRNi18n.h>

using namespace crn;

/*! Transforms an angle image into a RGB image
 * \ingroup imageother
 * \param[in]	img	the image to transform
 * \return	the new image
 */
ImageRGB crn::ColorWheel(const ImageAngle &img)
{
	auto res = ImageRGB(img.GetWidth(), img.GetHeight());
	for (auto tmp : Range(img))
		res.At(tmp) = pixel::HSV(img.At(tmp), 255, 255);
	return res;
}

