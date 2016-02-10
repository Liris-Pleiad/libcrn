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
 * file: CRNImageHSV.cpp
 * \author Yann LEYDIER
 */

#include <CRNi18n.h>
#include <CRNImage/CRNImageHSV.h>
#include <CRNException.h>

using namespace crn;

/*! \param[in]	img	the source image
 * \return	the hue channel
 */
ImageAngle crn::HChannel(const ImageHSV &img)
{
	auto chan = ImageAngle{img.GetWidth(), img.GetHeight()};
	for (auto tmp : Range(img))
		chan.At(tmp) = img.At(tmp).h;
	return chan;
}

/*! \param[in]	img	the source image
 * \return	the saturation channel
 */
ImageGray crn::SChannel(const ImageHSV &img)
{
	auto chan = ImageGray{img.GetWidth(), img.GetHeight()};
	for (auto tmp : Range(img))
		chan.At(tmp) = img.At(tmp).s;
	return chan;
}

/*! \param[in]	img	the source image
 * \return	the value channel
 */
ImageGray crn::VChannel(const ImageHSV &img)
{
	auto chan = ImageGray{img.GetWidth(), img.GetHeight()};
	for (auto tmp : Range(img))
		chan.At(tmp) = img.At(tmp).v;
	return chan;
}

/*! \param[in]	img	the source image
 * \return	the red channel
 */
ImageGray crn::RedChannel(const ImageHSV &img)
{
	auto chan = ImageGray{img.GetWidth(), img.GetHeight()};
	for (auto tmp : Range(img))
		chan.At(tmp) = pixel::RGB<uint8_t>(img.At(tmp)).r;
	return chan;
}

/*! \param[in]	img	the source image
 * \return	the green channel
 */
ImageGray crn::GreenChannel(const ImageHSV &img)
{
	auto chan = ImageGray{img.GetWidth(), img.GetHeight()};
	for (auto tmp : Range(img))
		chan.At(tmp) = pixel::RGB<uint8_t>(img.At(tmp)).g;
	return chan;
}

/*! \param[in]	img	the source image
 * \return	the blue channel
 */
ImageGray crn::BlueChannel(const ImageHSV &img)
{
	auto chan = ImageGray{img.GetWidth(), img.GetHeight()};
	for (auto tmp : Range(img))
		chan.At(tmp) = pixel::RGB<uint8_t>(img.At(tmp)).b;
	return chan;
}

