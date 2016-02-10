/* Copyright 2008-2016 INSA Lyon
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
 * file: CRNImageYUV.cpp
 * \author Jean DUONG
 */

#include <CRNImage/CRNImageYUV.h>
#include <CRNImage/CRNImage.h>
#include <CRNi18n.h>

using namespace crn;

ImageDoubleGray crn::YChannel(const ImageYUV &img)
{
	auto res = ImageDoubleGray(img.GetWidth(), img.GetHeight());
	for (auto tmp : Range(img))
		res.At(tmp) = img.At(tmp).y;
	return res;
}

ImageDoubleGray crn::UChannel(const ImageYUV &img)
{
	auto res = ImageDoubleGray(img.GetWidth(), img.GetHeight());
	for (auto tmp : Range(img))
		res.At(tmp) = img.At(tmp).u;
	return res;
}

ImageDoubleGray crn::VChannel(const ImageYUV &img)
{
	auto res = ImageDoubleGray(img.GetWidth(), img.GetHeight());
	for (auto tmp : Range(img))
		res.At(tmp) = img.At(tmp).v;
	return res;
}
