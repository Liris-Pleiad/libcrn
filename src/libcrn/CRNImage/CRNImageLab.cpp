/* Copyright 2008-2015 INSA Lyon
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
 * file: CRNImageLab.cpp
 * \author Yann LEYDIER
 */

#include <CRNImage/CRNImageLab.h>
#include <CRNImage/CRNImage.h>

using namespace crn;

ImageDoubleGray crn::LChannel(const ImageLab &img)
{
	auto res = ImageDoubleGray(img.GetWidth(), img.GetHeight());
	for (auto tmp : Range(img))
		res.At(tmp) = img.At(tmp).l;
	return res;
}

ImageDoubleGray crn::AChannel(const ImageLab &img)
{
	auto res = ImageDoubleGray(img.GetWidth(), img.GetHeight());
	for (auto tmp : Range(img))
		res.At(tmp) = img.At(tmp).a;
	return res;
}

ImageDoubleGray crn::BChannel(const ImageLab &img)
{
	auto res = ImageDoubleGray(img.GetWidth(), img.GetHeight());
	for (auto tmp : Range(img))
		res.At(tmp) = img.At(tmp).b;
	return res;
}

