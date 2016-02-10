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
 * file: CRNImage2DIntPolarVector.h
 * \author Yann LEYDIER
 */

#ifndef CRNIMAGE2DINTPOLARVECTOR_HEADER
#define CRNIMAGE2DINTPOLARVECTOR_HEADER

#include <CRNImage/CRNImage.h>
#include <CRNException.h>

/*! \defgroup imageother Misc images
 * \ingroup	image
 */

namespace crn
{
	/*! Creates a 2D-pixeled image from two images
	 * \ingroup	imageother
	 * \throws	ExceptionDimension	images do not have the same size
	 * \param[in]	x	the abscissa image
	 * \param[in]	y	the ordinate image
	 * \return	the new image
	 */
	template<typename T> Image<pixel::Cart2D<T>> MakeVector(const Image<T> &x, const Image<T> &y)
	{
		if ((x.GetWidth() != y.GetWidth()) || (x.GetHeight() != y.GetHeight()))
			throw ExceptionDimension("Image<pixel::Cart2D<T>> MakeVector(const Image<T> &x, const Image<T> &y): images do not have the same size.");
		auto res = Image<pixel::Cart2D<T>>(x.GetWidth(), x.GetHeight());
		for (auto tmp : Range(x))
			res.At(tmp) = {x.At(tmp), y.At(tmp)};
		return res;
	}

	/*! Extracts the rho channel
	 * \param[in]	the source image
	 * \return	the rho channel
	 */
	template<typename R, typename T> Image<R> RhoChannel(const Image<pixel::Polar2D<R, T>> &img)
	{
		auto res = Image<R>{img.GetWidth(), img.GetHeight()};
		for (auto tmp : Range(img))
			res.At(tmp) = img.At(tmp).rho;
		return res;
	}

	/*! Extracts the theta channel
	 * \param[in]	the source image
	 * \return	the theta channel
	 */
	template<typename R, typename T> Image<typename Unit<T>::type> ThetaChannel(const Image<pixel::Polar2D<R, T>> &img)
	{
		auto res = Image<typename Unit<T>::type>{img.GetWidth(), img.GetHeight()};
		for (auto tmp : Range(img))
			res.At(tmp) = img.At(tmp).theta.value;
		return res;
	}
}

#endif

