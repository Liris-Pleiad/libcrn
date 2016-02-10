/* Copyright 2006-2015 Yann LEYDIER, CoReNum, INSA-Lyon
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
 * file: CRNImageBW.h
 * \author Yann LEYDIER
 */

#ifndef CRNIMAGEBW_HEADER
#define CRNIMAGEBW_HEADER

#include <CRNImage/CRNImage.h>

/*! \defgroup imagebw Bitonal images
 * \ingroup image
 */
namespace crn
{
	class MatrixInt;
	class Histogram;

	/*! \addtogroup imagebw */
	/*@{*/
	/*! \brief Computes the left profile */
	Histogram LeftProfile(const ImageBW &img);
	/*! \brief Computes the right profile */
	Histogram RightProfile(const ImageBW &img);
	/*! \brief Computes the top profile */
	Histogram TopProfile(const ImageBW &img);
	/*! \brief Computes the bottom profile */
	Histogram BottomProfile(const ImageBW &img);
	/*! \brief Computes the horizontal projection */
	Histogram HorizontalProjection(const ImageBW &img);
	/*! \brief Computes the vertical projection */
	Histogram VerticalProjection(const ImageBW &img);

	/*! \brief Computes the vertical projection after rotation */
	Histogram VerticalSlantedProjection(const ImageBW &img, const Angle<Radian> &theta);

	/*! \brief Gets the mean horizontal black run */
	double MeanBlackRun(const ImageBW &img) noexcept;
	/*! \brief Gets the mean horizontal white run */
	double MeanWhiteRun(const ImageBW &img, int blackrun = -1) noexcept;
	/*! \brief Gets the mean vertical black run */
	double MeanBlackVRun(const ImageBW &img) noexcept;
	/*! \brief Returns the number of black pixels */
	size_t CountBlackPixels(const ImageBW &img) noexcept;
	/*! \brief Returns the number of white pixels */
	size_t CountWhitePixels(const ImageBW &img) noexcept;
	
	/*! \brief Removes isolated pixels and smooths edges */
	size_t Regularize(ImageBW &img, size_t min_neighbors = 0);

	/*! \brief Creates an image containing the distance transform */
	ImageIntGray DistanceTransform(const ImageBW &img, const MatrixInt &m1, const MatrixInt &m2);
	/*@}*/

}
#endif

